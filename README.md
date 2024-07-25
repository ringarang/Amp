# Amp
Client modification for popular MMO game ROBLOX to allow unsigned lua script execution via the ingame VM.

# What is this?
Amp is an old out-dated tool that allowed for end-users to create, edit and execute lua scripts within the ROBLOX lua environment. Typically this is protected and only allows signed scripts to execute in its environment. Amp allows for unrestricted and unsigned access to the ROBLOX lua API and execution within it taking advantage of prevalent flaws within the environments sandbox.

# How does it work?
To understand how Amp works it is required that one understands how the ROBLOX lua environment works and the flow of processes that allow a legitimately signed function prototype to operate on the lua VM. <br/>
Roblox uses their own lua implementation called lua-u which is open source on github. The first hurdle is creating a processor or "transpiler" to convert vanilla lua 5.1 function prototypes into ROBLOX-compatible ones this is done in client/Transpiler.cpp in which a vanilla lua 5.1 function is modified to be compatible with ROBLOX. This involves remapping normal lua 5.1 VM instructions into ROBLOX-compatible ones, this is done by reverse-engineering the ROVLOX VM to figure out the various OPCodes and registers each instruction contains. The normal lua instructions are then mapped to ROBLOX ones, relocated and then saved. <br/>
The next step of this process is the ROBLOX de-serializer which takes signed bytecode sent by the server upon game initialization, de-serializes it into a function prototype and then pushes it to the lua stack allowing for it to be called and executed. This process is entirely reversed and re-implemented into Amp to take a remapped and converted function prototype and write it into ROBLOX-compatible bytecode.<br/>
Now the easy part, this newly re-mapped prototype in ROBLOX compatible bytecode form is then fed to the ROBLOX deserializer as any normal signed script is and it is as simple as that. The resultant function prototype is then pushed to the stack and is ready to be scheduled to be executed.<br/>
While the method seems complicated, in tl;dr form it is simply replicating the ROBLOX bytecode format, preparing user-generated content using it and then feeding it to ROBLOX as if it were from the game server.

# Can I read up on this?
A vastly more intellectual member of the community known as Louka has a great paper on this method of execution within the ROBLOX scripting environment. It can be found here: https://github.com/the0d3v/LoukaPDF/blob/master/BytecodeConversion.pdf

# License
You can do whatever you want with this. This was made as a fun little passion project. See UNLICENSE for more details
