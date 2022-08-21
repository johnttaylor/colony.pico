using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Windows.Forms;

namespace Simulator
{
    public partial class MainForm : Form
    {
        public int m_width;
        public int m_height;
        public Bitmap m_lcd;
        public Bitmap m_rgbLED;
        public Simulator.SocketListener m_listener;
        public bool m_pressedStateA = false;
        public bool m_pressedStateB = false;
        public bool m_pressedStateX = false;
        public bool m_pressedStateY = false;

        public MainForm()
        {
            InitializeComponent();
            this.Icon = Properties.Resources.pico_display_image;
            m_width = LcdPanel.ClientRectangle.Width;
            m_height = LcdPanel.ClientRectangle.Height;
            m_lcd = new Bitmap(m_width, m_height);
            FillImage(m_lcd, Color.Black, m_width, m_height);
            LcdPanel.BackgroundImage = m_lcd;
            m_rgbLED = new Bitmap(RgbLED.ClientRectangle.Width, RgbLED.ClientRectangle.Height);
            FillImage(m_rgbLED, Color.Black, RgbLED.ClientRectangle.Width, RgbLED.ClientRectangle.Height);
            RgbLED.BackgroundImage = m_rgbLED;


            m_listener = new Simulator.SocketListener();

            m_listener.RegisterCommand(new Simulator.Write(this));
            m_listener.RegisterCommand(new Simulator.UpdateDisplay(this));
            m_listener.RegisterCommand(new Simulator.Led(this));
            m_listener.RegisterCommand(new Simulator.Exit());
            m_listener.RegisterCommand(new Simulator.Nop(this));

            new Thread(new ParameterizedThreadStart(Simulator.SocketListener.Start)).Start(m_listener);
        }

        public void UpdateLcd()
        {
            if (InvokeRequired)
            {
                Invoke(new MethodInvoker(UpdateLcd));
            }
            else
            {
                StatusStripLabel.Text = String.Format("MCU Elapsed Time: {0:00} {1:00}:{2:00}:{3:00}.{4:000}", m_listener.m_dd, m_listener.m_hh, m_listener.m_mm, m_listener.m_sec, m_listener.m_msec);
                LcdPanel.BackgroundImage = m_lcd;
                StatusStrip.Refresh();
                LcdPanel.Refresh();
                RgbLED.Refresh();
            }
        }

        public void FillImage(Bitmap image, Color new_color, int w, int h)
        {
            for (int x = 0; x < w; x++)
            {
                for (int y = 0; y < h; y++)
                {
                    image.SetPixel(x, y, new_color);
                }
            }
        }


        private void ChangeInButtonState()
        {
            if (m_listener.m_socketHandle != null)
            {
                string buttons = String.Format("^buttons A {0} B {1} X {2} Y {3}\n", 
                    m_pressedStateA ? "DOWN" : "up", 
                    m_pressedStateB ? "DOWN" : "up", 
                    m_pressedStateX ? "DOWN" : "up", 
                    m_pressedStateY ? "DOWN" : "up");
                byte[] msg = Encoding.ASCII.GetBytes(buttons);
                m_listener.m_socketHandle.Send(msg);
            }
        }

        private void MainForm_FormClosing(object sender, FormClosingEventArgs e)
        {
            m_listener.Stop();
        }

        /////////////////////////////////////
        private void AButton_MouseDown(object sender, MouseEventArgs e)
        {
            Process_AButtonMouseDown();
        }
        private void AButton_MouseUp(object sender, MouseEventArgs e)
        {
            Process_AButtonMouseUp();
        }
        private void Process_AButtonMouseDown()
        {
            if (!m_pressedStateA)
            {
                m_pressedStateA = true;
                ChangeInButtonState();
            }
        }
        private void Process_AButtonMouseUp()
        {
            if (m_pressedStateA)
            {
                m_pressedStateA = false;
                ChangeInButtonState();
            }
        }

        private void BButton_MouseDown(object sender, MouseEventArgs e)
        {
            Process_BButtonMouseDown();
        }
        private void BButton_MouseUp(object sender, MouseEventArgs e)
        {
            Process_BButtonMouseUp();
        }
        private void Process_BButtonMouseDown()
        {
            if (!m_pressedStateB)
            {
                m_pressedStateB = true;
                ChangeInButtonState();
            }
        }

        private void Process_BButtonMouseUp()
        {
            if (m_pressedStateB)
            {
                m_pressedStateB = false;
                ChangeInButtonState();
            }
        }


        private void XButton_MouseDown(object sender, MouseEventArgs e)
        {
            Process_XButtonMouseDown();
        }
        private void XButton_MouseUp(object sender, MouseEventArgs e)
        {
            Process_XButtonMouseUp();
        }
        private void Process_XButtonMouseDown()
        {
            if (!m_pressedStateX)
            {
                m_pressedStateX = true;
                ChangeInButtonState();
            }
        }

        private void Process_XButtonMouseUp()
        {
            if (m_pressedStateX)
            {
                m_pressedStateX = false;
                ChangeInButtonState();
            }
        }

        private void YButton_MouseDown(object sender, MouseEventArgs e)
        {
            Process_YButtonMouseDown(); 
        }
        private void YButton_MouseUp(object sender, MouseEventArgs e)
        {
            Process_YButtonMouseUp();
        }
        private void Process_YButtonMouseDown()
        {
            if (!m_pressedStateY)
            {
                m_pressedStateY = true;
                ChangeInButtonState();
            }
        }
        private void Process_YButtonMouseUp()
        {
            if (m_pressedStateY)
            {
                m_pressedStateY = false;
                ChangeInButtonState();
            }
        }

        /////////////////////////////////////
        private void MainForm_KeyDown(object sender, KeyEventArgs e)
        {
            switch (e.KeyCode)
            {
                case Keys.A:
                    Process_AButtonMouseDown();
                    break;
                case Keys.B:
                    Process_BButtonMouseDown();
                    break;
                case Keys.X:
                    Process_XButtonMouseDown();
                    break;
                case Keys.Y:
                    Process_YButtonMouseDown();
                    break;
                default:
                    break;
            }
        }

        private void MainForm_KeyUp(object sender, KeyEventArgs e)
        {
            switch( e.KeyCode )
            {
                case Keys.A:
                    Process_AButtonMouseUp();
                    break;
                case Keys.B:
                    Process_BButtonMouseUp();
                    break;
                case Keys.X:
                    Process_XButtonMouseUp();
                    break;
                case Keys.Y:
                    Process_YButtonMouseUp();
                    break;
                default:
                    break;
            }
        }
    }

    // Write LCD data command. Writes a 'rectangle' worth of data
    // Command Format AFTER the leading (without leading/trailing SOF/EOF framing characters)
    //
    // <DD> <HH:MM:SS.sss> writeLCDData <x0> <w> <y0> <h> <hexdata>
    // Where:
    //      <DD>                is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
    //      <HH:MM:SS.sss>      is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
    //      <x0>                Top/left X coordinate (in pixel coordinates) of the rectangle
    //      <w>                 Width (in display coordinates) of the rectangle. Note: <w> should always be greater than 0
    //      <y0>                Top/left Y coordinate (in pixel coordinates) of the rectangle
    //      <h>                 height (in display coordinates) of the rectangle. Note: <h> should always be greater than 0
    //      <hexdata>           Pixel data as 'ASCII HEX' String (with no spaces).  Each byte is SINGLE pixel
    //                          Pixel layout is row, then column:
    //                              First Byte is:    x0,y0.
    //                              Byte w is:        x0+w,y0
    //                              Byte w+1 is:      x0,y0+1
    //                              Byte (h*w) is:    x1,y1
    //
    // NOTE: Color/Pixel size assumes RGB332 color resolution
    public class Write : ICommand
    {
        private MainForm m_ui;

        public Write(MainForm ui)
        {
            m_ui = ui;
        }

        public string GetCommandName() { return "writeLCDData"; }
        public bool ExecuteCommand(string rawString, List<string> tokenizeString)
        {
            Console.WriteLine("PROCESSING: " + rawString);
            int x = int.Parse(tokenizeString[3]);
            int w = int.Parse(tokenizeString[4]);
            int y = int.Parse(tokenizeString[5]);
            int h = int.Parse(tokenizeString[6]);
            byte[] pixelBytes = HexStringToByteArray(tokenizeString[7]);

            int numPixels = w * h;
            int dataIndex = 0;
            for (int yidx = y; yidx < y + h; yidx++)
            {
                for (int xidx = x; xidx < x + w; xidx++)
                {
                    m_ui.m_lcd.SetPixel(xidx, yidx, ConvertColor(pixelBytes[dataIndex]));
                    dataIndex++;
                }
            }

            return true;
        }

        Color ConvertColor(int firmwareColor)
        {
            int red = ((firmwareColor & 0b1110000) << 0);
            int green = ((firmwareColor & 0b00011100) << 3);
            int blue = ((firmwareColor & 0b00000011) << 6);
            return Color.FromArgb(red, green, blue);
        }

        private byte[] HexStringToByteArray(string hexString)
        {
            int hexStringLength = hexString.Length;
            byte[] b = new byte[hexStringLength / 2];
            for (int i = 0; i < hexStringLength; i += 2)
            {
                int topChar = (hexString[i] > 0x40 ? hexString[i] - 0x37 : hexString[i] - 0x30) << 4;
                int bottomChar = hexString[i + 1] > 0x40 ? hexString[i + 1] - 0x37 : hexString[i + 1] - 0x30;
                b[i / 2] = Convert.ToByte(topChar + bottomChar);
            }
            return b;
        }
    }

    // Updates the LCD (i.e. the visible screen) with the latest LCD data
    // Command Format AFTER the leading (without leading/trailing SOF/EOF framing characters)
    //
    // <DD> <HH:MM:SS.sss> updateLCD 
    // Where:
    //      <DD>                is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
    //      <HH:MM:SS.sss>      is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
    public class UpdateDisplay : ICommand
    {
        private MainForm m_ui;

        public UpdateDisplay(MainForm ui)
        {
            m_ui = ui;
        }

        public string GetCommandName() { return "updateLCD"; }

        public bool ExecuteCommand(string rawString, List<string> tokenizeString)
        {
            Console.WriteLine("PROCESSING: " + rawString);
            m_ui.UpdateLcd();
            return true;
        }
    }

    // Sets the RBG LED state
    // Command Format AFTER the leading (without leading/trailing SOF/EOF framing characters)
    //
    // <DD> <HH:MM:SS.sss> writeRGBLed <r> <g> <b>
    // Where:
    //      <DD>                is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
    //      <HH:MM:SS.sss>      is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
    //      <r>                 The 'Red' value (0-255) for the RGB LED
    //      <g>                 The 'Green' value (0-255) for the RGB LED
    //      <b>                 The 'Blue' value (0-255) for the RGB LED
    public class Led : ICommand
    {
        private MainForm m_ui;

        public Led(MainForm ui)
        {
            m_ui = ui;
        }

        public string GetCommandName() { return "writeRGBLed"; }
        public bool ExecuteCommand(string rawString, List<string> tokenizeString)
        {
            Console.WriteLine("PROCESSING: " + rawString);

            int r = int.Parse(tokenizeString[3]);
            int g = int.Parse(tokenizeString[4]);
            int b = int.Parse(tokenizeString[5]);

            m_ui.FillImage(m_ui.m_rgbLED, Color.FromArgb(r, g, b), m_ui.m_rgbLED.Width, m_ui.m_rgbLED.Height);
            m_ui.UpdateLcd();

            return true;
        }
    }

    // Command terminates the simulator
    //
    // <DD> <HH:MM:SS.sss> terminate
    // Where:
    //      <DD>                is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
    //      <HH:MM:SS.sss>      is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
    public class Exit : ICommand
    {
        public string GetCommandName() { return "terminate"; }
        public bool ExecuteCommand(string rawString, List<string> tokenizeString)
        {
            Console.WriteLine("PROCESSING: " + rawString);
            return false;
        }
    }

    // A Null-Operation command. This command is typically used to update the
    // simulator with the latest time
    //
    // <DD> <HH:MM:SS.sss> no
    // Where:
    //      <DD>                is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
    //      <HH:MM:SS.sss>      is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
    public class Nop : ICommand
    {
        private MainForm m_ui;

        public Nop(MainForm ui)
        {
            m_ui = ui;
        }

        public string GetCommandName() { return "nop"; }
        public bool ExecuteCommand(string rawString, List<string> tokenizeString)
        {
            Console.WriteLine("PROCESSING: " + rawString);
            m_ui.UpdateLcd();
            return true;
        }
    }

}
