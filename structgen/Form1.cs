using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using ScintillaNET;

namespace structgen
{
    public partial class Form1 : Form
    {
        private struct structMember
        {
            public string name;
            public string type;
            public bool isPointer;
            public int bytes_level;
            public int offset;
        };

        Dictionary<string, structMember> membersMap = new Dictionary<string, structMember>();

        private void RefreshCode()
        {
            var items = from pair in membersMap
                        orderby pair.Value.offset ascending
                        select pair;

            foreach (KeyValuePair<string, structMember> pair in items)
            {
                Console.WriteLine("{0}: {1}", pair.Key, pair.Value.offset);
            }

            StringBuilder sb = new StringBuilder();
            sb.Append("/*\nGenerated using amp: structgen.\n*/\n\nstruct ");

            sb.Append(textBox1.Text);
            sb.Append(" {");
            if (membersMap.Keys.Count <= 0)
            {
                sb.Append("};");
                scintilla1.Text = sb.ToString();
                return;
            }
            sb.Append("\n");

            int bytes = 0;
            int pads = 0;

            foreach (KeyValuePair<string, structMember> pair in items)
            {
                int topad = pair.Value.offset - bytes;
                if (topad > 0)
                {
                    sb.Append("\tbyte pad");
                    sb.Append(pads); pads++;
                    sb.Append("[");
                    sb.Append(topad);
                    sb.Append("]; // covers ");
                    sb.Append(bytes);
                    sb.Append(" -> ");
                    sb.Append(bytes + topad);
                    sb.Append("\n");
                    bytes += topad;
                }
                else if (topad < 0)
                {
                    MessageBox.Show(string.Format("{0}'s offset is malformated ({1})", pair.Value.name, topad), "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    membersMap.Remove(pair.Value.name);
                    listBox1.Items.Remove(pair.Value.name);
                    RefreshCode();
                    return;
                }

                sb.Append("\t");
                sb.Append(pair.Value.type);
                if (!pair.Value.isPointer) {                    
                    bytes += pair.Value.bytes_level;                 
                }
                else {
                    bytes += 4;
                    sb.Append('*', pair.Value.bytes_level);                    
                }
                sb.Append(" ");
                sb.Append(pair.Value.name);
                sb.Append("; // *reinterpret_cast<");
                sb.Append(pair.Value.type);
                if (pair.Value.isPointer) sb.Append('*', pair.Value.bytes_level);
                sb.Append("*>(a1 + ");
                sb.Append(pair.Value.offset);
                sb.Append(");\n");
            }

            sb.Append("};");
            scintilla1.Text = sb.ToString();
        }

        public Form1()
        {
            InitializeComponent();
        }

        private void SetupEditor(Scintilla editor)
        {
            editor.Dock = DockStyle.Fill;

            editor.WrapMode = WrapMode.None;
            editor.IndentationGuides = IndentView.LookBoth;
            editor.CaretLineVisible = true;
            editor.CaretLineBackColorAlpha = 50;
            editor.CaretLineBackColor = Color.CornflowerBlue;
            editor.TabWidth = editor.IndentWidth = 4;
            editor.Margins[0].Width = 16;
            editor.ViewWhitespace = WhitespaceMode.Invisible;
            editor.UseTabs = true;

            Font editorFont = new Font(FontFamily.GenericMonospace, 10.0f, FontStyle.Regular);
            editor.StyleResetDefault();
            editor.Styles[Style.Default].Font = editorFont.Name;
            editor.Styles[Style.Default].Size = (int)editorFont.SizeInPoints;
            editor.StyleClearAll();

            editor.Lexer = ScintillaNET.Lexer.Cpp;

            editor.Styles[Style.Cpp.Default].ForeColor = Color.Silver;
            editor.Styles[Style.Cpp.Comment].ForeColor = Color.Green;
            editor.Styles[Style.Cpp.CommentLine].ForeColor = Color.Green;
            editor.Styles[Style.Cpp.Number].ForeColor = Color.Olive;
            editor.Styles[Style.Cpp.Word].ForeColor = Color.Blue;
            editor.Styles[Style.Cpp.Word2].ForeColor = Color.Purple;
            editor.Styles[Style.Cpp.String].ForeColor = Color.FromArgb(163, 21, 21);
            editor.Styles[Style.Cpp.Character].ForeColor = Color.FromArgb(163, 21, 21);
            editor.Styles[Style.Cpp.Verbatim].ForeColor = Color.FromArgb(163, 21, 21);
            editor.Styles[Style.Cpp.Identifier].ForeColor = Color.Black;
            editor.Styles[Style.Cpp.StringEol].BackColor = Color.Pink;
            editor.Styles[Style.Cpp.Operator].ForeColor = Color.Purple;
            editor.Styles[Style.Cpp.Preprocessor].ForeColor = Color.Maroon;

            editor.Styles[Style.Cpp.CommentDoc].ForeColor = Color.Purple;
            editor.Styles[Style.Cpp.CommentDocKeyword].ForeColor = Color.Red;
            editor.Styles[Style.Cpp.CommentDocKeywordError].ForeColor = Color.Red;
            editor.Styles[Style.Cpp.CommentLineDoc].ForeColor = Color.Purple;

            editor.SetKeywords(0, "abstract as base break case catch checked continue default delegate do else event explicit extern false finally fixed for foreach goto if implicit in interface internal is lock namespace new null object operator out override params private protected public readonly ref return sealed sizeof stackalloc switch this throw true try typeof unchecked unsafe using virtual while");
            editor.SetKeywords(1, "unsigned int32_t int16_t int8_t uint32_t uint16_t uint8_t uintptr_t bool byte char class const decimal double enum float int long sbyte short static string struct uint ulong ushort void");
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            scintilla1.Lexer = ScintillaNET.Lexer.Cpp;
            SetupEditor(scintilla1);
        }

        private void checkBox1_CheckedChanged(object sender, EventArgs e)
        {
            if (checkBox1.Checked) label4.Text = "level:";
            else label4.Text = "bytes:";
        }

        private void button2_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(textBox2.Text) || string.IsNullOrWhiteSpace(comboBox1.Text))
            {
                MessageBox.Show("Name or Type cannot be empty.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            structMember nMember;
            nMember.bytes_level = decimal.ToInt32(numericUpDown1.Value);
            numericUpDown1.Value = 0;
            nMember.isPointer = checkBox1.Checked;
            checkBox1.Checked = false;
            nMember.name = textBox2.Text;
            textBox2.Text = string.Empty;
            nMember.offset = decimal.ToInt32(numericUpDown2.Value);
            numericUpDown2.Value = 0;
            nMember.type = comboBox1.Text;
            comboBox1.Text = string.Empty;

            if (membersMap.ContainsKey(nMember.name))
            {
                MessageBox.Show("This member already exists.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                return;
            }

            if (!nMember.isPointer)
            {
                switch (nMember.type)
                {
                    case "int":
                    case "float":
                    case "int32_t":
                    case "uint32_t":
                    case "uintptr_t":
                    case "unsigned int":
                    case "DWORD":
                        nMember.bytes_level = 4;
                        break;
                    case "uint8_t":
                    case "int8_t":
                    case "char":
                    case "byte":
                    case "unsigned char":
                    case "bool":
                        nMember.bytes_level = 1;
                        break;
                    case "double":
                        nMember.bytes_level = 8;
                        break;
                    case "int16_t":
                    case "uint16_t":
                    case "WORD":
                        nMember.bytes_level = 2;
                        break;
                    default:
                        break;
                };

                if (nMember.bytes_level <= 0)
                {
                    MessageBox.Show("Your member's size must be above or equal to 1 byte.", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    return;
                }
            }
            membersMap.Add(nMember.name, nMember);
            listBox1.Items.Add(nMember.name);

            RefreshCode();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            if (listBox1.Items.Count <= 0) return;
            if (listBox1.SelectedItem == null) listBox1.SelectedItem = listBox1.Items[listBox1.Items.Count-1];

            membersMap.Remove(listBox1.SelectedItem.ToString());
            listBox1.Items.Remove(listBox1.SelectedItem);
            RefreshCode();
        }

        private void textBox1_TextChanged(object sender, EventArgs e)
        {
            RefreshCode();
        }
    }
}
