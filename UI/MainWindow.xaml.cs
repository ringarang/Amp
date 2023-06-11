using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using System.Reflection;

namespace UI
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    
    public partial class MainWindow : Window
    {
        
        public MainWindow()
        {
            InitializeComponent();
        }

        private void Window_MouseDown(object sender, MouseButtonEventArgs e)
        {
            // Drag/Move the window (self explanatory)
            if (e.ChangedButton == MouseButton.Left)
                DragMove();
        }

        private void textBlock_Loaded(object sender, RoutedEventArgs e)
        {
            // Get text from "texts.txt" and apply to "pp" string
            var assembly = Assembly.GetExecutingAssembly();
            var resname = "UI.texts.txt";
            string pp;
            using (Stream stream = assembly.GetManifestResourceStream(resname))
            using (StreamReader reader = new StreamReader(stream))
            {
                pp = reader.ReadToEnd();
            }

            // Split and randomy select string from "pp" and set caption text
            string[] texts = pp.Split(Convert.ToChar("/"));
            Random rand = new Random();
            int index = rand.Next(texts.Length);
            splash.Text = ($"{texts[index]}");
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            // create "user" and "pass" vars
            string user = textBox.Text;
            string pass = passwordBox.Password;

            //do the check if either is blank
            if (user.Length == 0 | pass.Length == 0) { 
                MessageBox.Show("Username or Password CANNOT be blank!", "Oops!", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }
            if(user == "user" && pass == "pass")
            {
                MessageBox.Show("Correct User and Pass!");
            }
            else
            {
                MessageBox.Show("Incorrect User or Pass!");
            }
        }

        private void button_Click_1(object sender, RoutedEventArgs e)
        {
            Application.Current.Shutdown();
        }
    }
}
