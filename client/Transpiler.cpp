#include "Transpiler.hpp"

#include <sstream>
#include <string>
#include <limits>
#include <unordered_map>

extern "C" {
#include "Lua\lua.h"
#include "Lua\lualib.h"
#include "Lua\lauxlib.h"
#include "Lua\luaconf.h"
#include "Lua\llimits.h"
#include "Lua\lobject.h"
#include "Lua\lcode.h"
#include "Lua\lopcodes.h"
#include "Lua\lstate.h"
}

#include "Addresses.hpp"
#include "ConsoleHandler.hpp"

enum BytecodeResult : std::uint8_t {
	FAILED = 0x00,
	SUCCEED = 0x01
};

/* Static's opcode list */
enum Luau : std::uint8_t
{
	NOOP = 0x00,
	MARKUPVAL = 0x12,
	INITVA = 0xA3,
	INIT = 0xC0,
	MOVE = 0x52,
	LOADK = 0x6F,
	LOADNIL = 0xC6,
	LOADBOOL = 169,
	LOADTABLE = 0xE2,
	LOADINT = 0x8C,
	NEWTABLE = 0xFF,
	GETUPVAL = 0xFB,
	GETGLOBAL = 0x35,
	GETGLOBALOPT = 0xA4,
	GETTABLE = 0x87,
	GETTABLEK = 0x4D,
	GETTABLEN = 0x13,
	SETUPVAL = 0xDE,
	SETTABLE = 0x6A,
	SETGLOBAL = 0x18,
	SETTABLEK = 0x30,
	SETLIST = 0xC5,
	UNM = 0x39,
	NOT = 0x56,
	LEN = 0x1C,
	CONCAT = 0x73,
	TFORLOOP = 0x6E,
	FORPREP = 0xA8,
	FORLOOP = 0x8B,
	PAIRSPREP = 0x17,
	PAIRSLOOP = 0xFA,
	SELF = 0xBC,
	ADD = 0x43,
	SUBK = 0x78,
	MULK = 0x5B,
	DIVK = 0x3E,
	POWK = 0x04,
	MODK = 0x21,
	ADDK = 0x95,
	SUB = 0x26,
	MUL = 0x09,
	DIV = 0xEC,
	POW = 0xB2,
	MOD = 0xCF,
	JMP = 0x65,
	MEDIUMJMP = 0x43,
	LONGJMP = 0x69,
	EQ = 0xF1,
	NEQ = 0x9A,
	LT = 0xB7,
	GT = 0x60,
	LE = 0xD4,
	GE = 0x7D,
	AND = 0xAD,
	TESTJMP = 0x2B,
	TEST = 0x0E,
	CALL = 0x9F,
	VARARG = 0xDD,
	CLOSURE = 0xD9,
	CLOSE = 0xC1,
	RETURN = 0x82,
};

struct relocation_index {
	int luaPC;
	int luaUPC;
	int Extra;
	Instruction luaInstruction;
	relocation_index(int luaPC, int luaUPC, Instruction luaInstruction, int Extra) :
		luaPC(luaPC),
		luaUPC(luaUPC),
		luaInstruction(luaInstruction),
		Extra(Extra)
	{}
};

using TStringMap = std::unordered_map<TString*, int>;
using TStringMapValue = TStringMap::value_type;

using ProtoMap = std::unordered_map<Proto*, int>;
using ProtoMapValue = ProtoMap::value_type;

#pragma pack(push, 1)
union r_instruction { //Thanks iivillian
	struct {
		Luau op;
		uint8_t a;
		union {
			struct {
				uint8_t b, c;
			};
			uint16_t Bx;
			int16_t sBx;
		};
	};
	uint32_t value;
	r_instruction() {}
	r_instruction(Luau op, uint8_t a, uint8_t b)
		: op(op), a(a), b(b) {}
	r_instruction(Luau op, uint8_t a, uint8_t b, uint8_t c)
		: op(op), a(a), b(b), c(c) {}
	r_instruction(Luau op, uint8_t a, uint16_t Bx)
		: op(op), a(a), Bx(Bx) {}
	r_instruction(Luau op, uint8_t a, int16_t sBx)
		: op(op), a(a), sBx(sBx) {}
	r_instruction(uint16_t psuedo_instruction)
		: value(psuedo_instruction) {}
	explicit operator Instruction() const { return value; }
};
#pragma pack(pop)

enum ConstantType : std::uint8_t {
	nil = 0,
	boolean = 1,
	number = 2,
	str = 3,
	global = 4
};

namespace utils {
	template <class tt>
	void write(std::ostringstream& stream, const tt v) {
		stream.write(reinterpret_cast<const char*>(&v), sizeof(v));
	}

	template <class tt>
	void writeCompressed(std::ostringstream& stream, tt v) {
		do {
			std::uint8_t nv = v & std::numeric_limits<char>::max();
			v >>= 7;
			if (v) nv |= (std::numeric_limits<char>::max() + 1);
			write<std::uint8_t>(stream, nv);
		} while (v);
	}

	void writeString(std::ostringstream& stream, const char* s, const std::uint32_t sz) {
		for (auto i = 0; i < std::string(s).size(); i++) {
			write<std::int8_t>(stream, s[i]);
		}
	}

	template <class tt0, class tt1>
	const bool keyAlreadyExists(std::unordered_map<tt0, tt1> m, tt0 v) {
		return m.find(v) != m.end();
	}
}

void doTStringMap(TStringMap& tsMap, std::vector<TString*>& string_vector, const Proto* p) {	
	for (std::uint32_t it = 0; it < p->sizek; it++)	if (p->k[it].tt == LUA_TSTRING) {
		TString* ts =  &p->k[it].value.gc->ts;
		if (!utils::keyAlreadyExists(tsMap, ts)) {
			tsMap.insert(TStringMapValue(ts, string_vector.size()));
			string_vector.push_back(ts);
		}
	}		
	for (std::uint32_t it = 0; it < p->sizep; it++) doTStringMap(tsMap, string_vector, p->p[it]);
}

void writeTStringMap(std::ostringstream& stream, std::vector<TString*> string_vector, TStringMap& tsMap) {
	utils::writeCompressed(stream, string_vector.size());
	for (std::uint32_t i = 0; i < string_vector.size(); ++i) {
		const auto str = string_vector[i];
		utils::writeCompressed(stream, str->tsv.len);
		utils::writeString(stream, getstr(str), str->tsv.len);
	}
}

void doProtoMap(ProtoMap& pMap, std::vector<Proto*>& proto_vector, const Proto* p) {
	for (std::uint32_t it = 0; it < p->sizep; it++) 
		doProtoMap(pMap, proto_vector, p->p[it]);

	pMap.insert(ProtoMapValue(const_cast<Proto*>(p), proto_vector.size()));	
	proto_vector.push_back(const_cast<Proto*>(p));
}

void convertInstructions(std::vector<r_instruction>& program, const Proto* p) {
	std::unordered_map<int, int> pc_map;

	std::vector<relocation_index> reloc;
	
	program.emplace_back(Luau::INITVA, p->numparams, 0x00, 0x00);

	bool needs_close = false;

	for (std::uint32_t iter = 0; iter < p->sizecode; iter++) {
		pc_map.insert({ iter, program.size() });
		const Instruction instr = p->code[iter];
		const std::uint8_t opcode = GET_OPCODE(instr);
		const std::uint8_t a = GETARG_A(instr);
		const std::uint8_t b = GETARG_B(instr);
		const std::uint16_t bx = GETARG_Bx(instr);
		const std::uint16_t sbx = GETARG_sBx(instr);
		const std::uint8_t c = GETARG_C(instr);

		switch (opcode) {
		case OpCode::OP_SELF: {
			program.emplace_back(Luau::MOVE, a+1, b, c);
			program.emplace_back(Luau::GETTABLE, a, b, c); 
			break; 
		}
		case OpCode::OP_SETGLOBAL: {
			program.emplace_back(Luau::SETGLOBAL, a, 0, 0);
			program.emplace_back(bx);
			break;
		}
		case OpCode::OP_NEWTABLE: {
			program.emplace_back(Luau::NEWTABLE, a, 0, 0);
			program.emplace_back(0);
			break;
		}
		case OpCode::OP_SETLIST: {
			//basically just remake 5.1 setlist
			program.emplace_back(Luau::SETLIST, a, a + 1, b == 0 ? 0 : b + 1);
			program.emplace_back(static_cast<int16_t>((c - 1) * LFIELDS_PER_FLUSH + 1));
			break;
		}
		case OpCode::OP_CONCAT:
			program.emplace_back(Luau::CONCAT, a, b, c);
			break;
		case OpCode::OP_MOVE:
			program.emplace_back(Luau::MOVE, a, b, c);
			break;
		case OpCode::OP_LOADK: {
			//std::uint32_t idx = INDEXK(bx);
			//if (p->k[idx].tt == LUA_TNUMBER) 
			//	if (p->k[idx].value.n < std::numeric_limits<std::int16_t>::max() && p->k[idx].value.n > std::numeric_limits<std::int16_t>::min()) 
			//		program.emplace_back(Luau::LOADINT, a, (std::int16_t)p->k[idx].value.n);
			//	else program.emplace_back(Luau::LOADK, a, bx);
			//else 
			program.emplace_back(Luau::LOADK, a, bx);
			break;
		}
		case OpCode::OP_LOADBOOL:
			program.emplace_back(Luau::LOADBOOL, a, b, c);
			break;
		case OpCode::OP_LOADNIL: {
			std::int32_t it = b;
			while (it >= a) { program.emplace_back(Luau::LOADNIL, it--, 0, 0); }
			break;
		}
		case OpCode::OP_FORPREP: {
			program.emplace_back(Luau::MOVE, a + 3, a + 0, 0);
			program.emplace_back(Luau::MOVE, a + 0, a + 1, 0);
			program.emplace_back(Luau::MOVE, a + 1, a + 2, 0);
			program.emplace_back(Luau::MOVE, a + 2, a + 3, 0);
				
			program.emplace_back(Luau::FORPREP, a, static_cast<int16_t>(NULL));
			reloc.emplace_back(iter, program.size() - 1, instr, 0);
				
			program.emplace_back(Luau::MOVE, a + 3, a + 2, 0);
			break;
		}
		case OpCode::OP_FORLOOP: {
			program.emplace_back(Luau::FORLOOP, a, static_cast<int16_t>(NULL));
			reloc.emplace_back(iter, program.size() - 1, instr, -2);
			break;
		}
		case OpCode::OP_TEST: {
			program.emplace_back(c == 1 ? Luau::TEST : Luau::TESTJMP, a, static_cast<int16_t>(1));
			break;
		}
		case OpCode::OP_TESTSET: {
			program.emplace_back(c == 1 ? Luau::TEST : Luau::TESTJMP, b, static_cast<int16_t>(2));
			program.emplace_back(Luau::MOVE, a, b, 0);
			break;
		}
		case OpCode::OP_EQ: {
			program.emplace_back(a == 0 ? Luau::EQ : Luau::NEQ, b, static_cast<int16_t>(2));
			program.emplace_back(c);
			break;
		}
		case OpCode::OP_LT: {
			program.emplace_back(a == 0 ? Luau::LT : Luau::GT, b, static_cast<int16_t>(2));
			program.emplace_back(c);
			break;
		}
		case OpCode::OP_LE: {
			program.emplace_back(a == 0 ? Luau::LE : Luau::GE, b, static_cast<int16_t>(2));
			program.emplace_back(c);
			break;
		}
		case OpCode::OP_LEN: {
			program.emplace_back(Luau::LEN, a, b, c);
			break;
		}
		case OpCode::OP_UNM: {
			program.emplace_back(Luau::UNM, a, b, c);
			break;
		}
		case OpCode::OP_NOT: {
			program.emplace_back(Luau::NOT, a, b, c);
			break;
		}
		case OpCode::OP_TFORLOOP: {
			program.emplace_back(Luau::TFORLOOP, a, static_cast<int16_t>(2));
			program.emplace_back(c);
			program.emplace_back(Luau::JMP, 0, static_cast<int16_t>(1));
			break;
		}
		case OpCode::OP_GETGLOBAL:
			program.emplace_back(Luau::GETGLOBAL, a, 0, 0);
			program.emplace_back(bx);
			break;
		case OpCode::OP_TAILCALL:
		case OpCode::OP_CALL: {
			program.emplace_back(Luau::CALL, a, b, c);
			break;
		}
		case OpCode::OP_CLOSE:
			program.emplace_back(Luau::CLOSE, a, b, c);
			break;
		case OpCode::OP_RETURN:
			if(needs_close)
				program.emplace_back(Luau::CLOSE, 0, 0, 0);
			program.emplace_back(Luau::RETURN, a, b, c);
			break;
		case OpCode::OP_CLOSURE: { 
			program.emplace_back(Luau::CLOSURE, a, bx);

			for (auto i = 0; i < p->p[bx]->nups; i++) { // Bx of closure instruction holds the index of proto being accessed in lproto->p (childproto)
				needs_close = true;
				auto upvalue_instruction = p->code[iter + i + 1];
				program.emplace_back(Luau::MARKUPVAL, GET_OPCODE(upvalue_instruction) == OP_GETUPVAL ? 2 : 1, GETARG_B(upvalue_instruction), 0);
			}
			iter += p->p[bx]->nups;
			break;
		}
		case OpCode::OP_VARARG:
			program.emplace_back(Luau::VARARG, a, b, c);
			break;
		case OpCode::OP_GETUPVAL:
			program.emplace_back(Luau::GETUPVAL, a, b, c);
			break;
		case OpCode::OP_SETUPVAL:
			program.emplace_back(Luau::SETUPVAL, a, b, c);
			break;
		case OpCode::OP_GETTABLE: { // If these have issues then remember that it is luaK_exp2RK in lcode.c
			program.emplace_back(Luau::GETTABLE, a, b, c);
			break;
		}
		case OpCode::OP_SETTABLE: {
			program.emplace_back(Luau::SETTABLE, c, a, b);
			break;
		}
		case OpCode::OP_ADD: {
			program.emplace_back(Luau::ADD, a, b, c);
			break;
		}
		case OpCode::OP_SUB: {
			program.emplace_back(Luau::SUB, a, b, c);
			break;
		}
		case OpCode::OP_MUL: {
			program.emplace_back(Luau::MUL, a, b, c);
			break;
		}
		case OpCode::OP_DIV: {
			program.emplace_back(Luau::DIV, a, b, c);
			break;
		}
		case OpCode::OP_POW: {
			program.emplace_back(Luau::POW, a, b, c);
			break;
		}
		case OpCode::OP_MOD: {
			program.emplace_back(Luau::MOD, a, b, c);
			break;
		}
		case OpCode::OP_JMP: {
			program.emplace_back(Luau::JMP, a, bx);
			reloc.emplace_back(iter, program.size() - 1, instr,-1);
			break;
		}
		default:
			ConsoleHandler::Send("[WARNING] Unsupported opcode %s.\n", luaP_opnames[opcode]);			
			break; 
		}
	}

	for (relocation_index& rel : reloc) {
		const std::int32_t instr = GETARG_sBx(rel.luaInstruction) + rel.luaPC + 1; //calculates the luapc that was where we would jump to before added psuedoinstructions
		std::int32_t sbxDifference = pc_map.find(instr)->second - rel.luaUPC ; // difference between vanilla and luau pc
		program[rel.luaUPC].sBx = sbxDifference + rel.Extra;
	}
}

void writeProtoMap(std::ostringstream& stream, std::vector<Proto*>& proto_vector, TStringMap& tsMap, ProtoMap& pMap) {
	utils::writeCompressed(stream, pMap.size());
	for(std::int32_t i = 0; i < proto_vector.size(); ++i) {
		const Proto* currentP = proto_vector[i];
		utils::writeCompressed(stream, currentP->maxstacksize);
		utils::writeCompressed(stream, currentP->numparams);
		utils::writeCompressed(stream, currentP->nups);
		utils::writeCompressed(stream, currentP->is_vararg);

		std::vector<r_instruction> program = {};
		convertInstructions(program, currentP);
		utils::writeCompressed(stream, program.size());
		for (std::uint32_t it0 = 0; it0 < program.size(); it0++) {
			utils::write<std::uint32_t>(stream, program[it0].value);
		}

		utils::writeCompressed(stream, currentP->sizek);
		for (std::uint32_t it0 = 0; it0 < currentP->sizek; it0++) {
			TValue* constant = &currentP->k[it0];

			switch (constant->tt)
			{
			case LUA_TNIL: {
				utils::write<std::uint8_t>(stream, ConstantType::nil);
				break;
			}
			case LUA_TBOOLEAN: {
				utils::write<std::uint8_t>(stream, ConstantType::boolean);
				utils::write<std::uint8_t>(stream, bvalue(constant));
				break;
			}
			case LUA_TNUMBER: {
				utils::write<std::uint8_t>(stream, ConstantType::number);
				utils::write<double>(stream, nvalue(constant));
				break;
			}
			case LUA_TSTRING: {
				utils::write<std::uint8_t>(stream, ConstantType::str);
				utils::writeCompressed(stream, tsMap[rawtsvalue(constant)]+1); 
				break;
			}
			}
		}

		utils::writeCompressed(stream, currentP->sizep);
		for (std::uint32_t it0 = 0; it0 < currentP->sizep; it0++)
			utils::writeCompressed(stream, pMap[currentP->p[it0]]);

		utils::write<std::uint8_t>(stream, 0); // unk
		
		utils::write<std::uint8_t>(stream, 0); // lineinfo (todo)

		utils::write<std::uint8_t>(stream, 0); // unk

	}
}

std::string Amp::Transpiler::Serialize(const char* src)
{
	lua_State* compileState = luaL_newstate();
	Proto* p = nullptr;
	const char* errmsg = nullptr;
	luaL_openlibs(compileState);
	{
		if (luaL_loadstring(compileState, src)) errmsg = lua_tostring(compileState, -1); else p = clvalue(compileState->top - 1)->l.p;
	}

	std::ostringstream bcStream;
	{
		if (p != nullptr) {
			TStringMap tsMap;
			ProtoMap pMap;
			std::vector<TString*> string_vector;
			std::vector<Proto*> proto_vector;

			utils::write<std::uint8_t>(bcStream, BytecodeResult::SUCCEED);

			doTStringMap(tsMap, string_vector, p);
			writeTStringMap(bcStream, string_vector, tsMap);

			doProtoMap(pMap,proto_vector, p);
			writeProtoMap(bcStream,proto_vector, tsMap, pMap); 

			utils::writeCompressed(bcStream, pMap.size() - 1); //entry point
		}
		else {
			utils::write<std::uint8_t>(bcStream, BytecodeResult::FAILED);
			utils::writeString(bcStream, errmsg, strlen(errmsg));
		}
	}
	std::string bcStreamRes = bcStream.str();
	lua_close(compileState);
	return bcStreamRes;
}
