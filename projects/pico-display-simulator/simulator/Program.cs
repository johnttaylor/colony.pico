﻿using Simulator;
using System;
using System.Collections.Generic;
using System.Drawing;
using System.Windows.Forms;

namespace Simulator
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new MainForm());
        }
    }
}

namespace Simulator
{
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
    //      <hexdata>           Pixel data as 'ASCII HEX' String (upper case and with no spaces).  Each PIXEL is two bytes in LITTLE ENDIAN ordering
    //                          Pixel layout is row, then column:
    //                              First Pixel is:   x0, y0
    //                              Pixel w is:       x0+w, y0
    //                              Pixel w+1 is:     x0, y0+1
    //                              Pixel (h*w) is:   x1, y1
    //
    // NOTE: Color/Pixel size assumes RGB565 color resolution
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
            byte[] pixelBytes = Utils.HexStringToByteArray(tokenizeString[7]);

            int numPixels = w * h;
            int dataIndex = 0;
            for (int yidx = y; yidx < y + h; yidx++)
            {
                for (int xidx = x; xidx < x + w; xidx++)
                {
                    // Pixel byte ordering is assumed to be Little Endian
                    m_ui.m_lcd.SetPixel(xidx, yidx, Utils.ConvertColor(pixelBytes[dataIndex] + (pixelBytes[dataIndex + 1] << 8)));
                    dataIndex += 2;
                }
            }

            return true;
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

    // Sets the entire LCD to the specified color and updates the visible screen
    // Command Format AFTER the leading (without leading/trailing SOF/EOF framing characters)
    //
    // <DD> <HH:MM:SS.sss> fillLCD <color> 
    // Where:
    //      <DD>                is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
    //      <HH:MM:SS.sss>      is CPU time since power-up/reset:  Format is: DD HH:MM:SS.sss
    //      <color>             Pixel color data as 'ASCII HEX' String (upper case and with no spaces).  Color data is two bytes in LITTLE ENDIAN ordering
    //
    // NOTE: Color is assumed to be RGB565 color resolution

    public class FillLCD : ICommand
    {
        private MainForm m_ui;

        public FillLCD(MainForm ui)
        {
            m_ui = ui;
        }

        public string GetCommandName() { return "fillLCD"; }

        public bool ExecuteCommand(string rawString, List<string> tokenizeString)
        {
            Console.WriteLine("PROCESSING: " + rawString);
            byte[] colorBytes = Utils.HexStringToByteArray(tokenizeString[3]);
            m_ui.FillImage( m_ui.m_lcd, Utils.ConvertColor(colorBytes[0] + (colorBytes[1] << 8)), m_ui.m_width, m_ui.m_height );    
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

    // Common/Helper methods
    public class Utils
    {
        static public Color ConvertColor(int firmwareColor)
        {
            int red = (firmwareColor & 0b1111100000000000) >> 8;
            int green = (firmwareColor & 0b0000011111100000) >> 3;
            int blue = (firmwareColor & 0b0000000000011111) << 3;
            return Color.FromArgb(red, green, blue);
        }

        static public byte[] HexStringToByteArray(string hexString)
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
}

