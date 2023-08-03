using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Data.Common;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using static System.Net.Mime.MediaTypeNames;
using static System.Windows.Forms.LinkLabel;

namespace Audio_File_Calibration_Tool
{
    public partial class Home : Form
    {
        string audioFileName;
        int audioFileSize;
        byte[] wavFile;
        string calibration_tool_version = "00.1";
        byte[] reservedData = new byte[36];
        DataTable table = new DataTable(); 
        DateTime currentTime = DateTime.Now;
        byte[] ShiftDenominator;
        int km = 100;

        // KM - Gain - Freq
        int[,] default_table_settings = new int[,]
        {
            { 00, 00 , 00 }, // 0 dB (mute)

            { 01, 1000 , 01 }, // 47.2 dB - 47.2 =  0.0 dB 
            { 02, 1072 , 02 }, // 47.8 dB - 47.2 =  0.6 dB
            { 03, 1148 , 03 }, // 48.4 dB - 47.2 =  1.2 dB
            { 04, 1230 , 04 }, // 49.0 dB - 47.2 =  1.8 dB
            { 05, 1318 , 05 }, // 49.6 dB - 47.2 =  2.4 dB
            { 06, 1413 , 06 }, // 50.2 dB - 47.2 =  3.0 dB
            { 07, 1514 , 07 }, // 50.8 dB - 47.2 =  3.6 dB
            { 08, 1622 , 08 }, // 51.4 dB - 47.2 =  4.2 dB
            { 09, 1738 , 09 }, // 52.0 dB - 47.2 =  4.8 dB
            { 10, 1862 , 10 }, // 52.6 dB - 47.2 =  5.4 dB

            { 11, 1995, 11 }, // 53.2 dB - 47.2 =  6.0 dB
            { 12, 2138, 12 }, // 53.8 dB - 47.2 =  6.6 dB
            { 13, 2291, 13 }, // 54.4 dB - 47.2 =  7.2 dB
            { 14, 2455, 14 }, // 55.0 dB - 47.2 =  7.8 dB
            { 15, 2630, 15 }, // 55.6 dB - 47.2 =  8.4 dB
            { 16, 2818, 16 }, // 56.2 dB - 47.2 =  9.0 dB
            { 17, 3020, 17 }, // 56.8 dB - 47.2 =  9.6 dB
            { 18, 3236, 18 }, // 57.4 dB - 47.2 = 10.2 dB
            { 19, 3467, 19 }, // 58.0 dB - 47.2 = 10.8 dB
            { 20, 3715, 20 }, // 58.6 dB - 47.2 = 11.4 dB

            { 21, 0 , 20 },
            { 22, 0 , 20 },
            { 23, 0 , 20 },
            { 24, 0 , 20 },
            { 25, 0 , 20 },
            { 26, 0 , 20 },
            { 27, 0 , 20 },
            { 28, 0 , 20 },
            { 29, 0 , 20 },
            { 30, 0 , 20 },

            { 31, 0 , 20 },
            { 32, 0 , 20 },
            { 33, 0 , 20 },
            { 34, 0 , 20 },
            { 35, 0 , 20 },
            { 36, 0 , 20 },
            { 37, 0 , 20 },
            { 38, 0 , 20 },
            { 39, 0 , 20 },
            { 40, 0 , 20 },

            { 41, 0 , 20 },
            { 42, 0 , 20 },
            { 43, 0 , 20 },
            { 44, 0 , 20 },
            { 45, 0 , 20 },
            { 46, 0 , 20 },
            { 47, 0 , 20 },
            { 48, 0 , 20 },
            { 49, 0 , 20 },
            { 50, 0 , 20 },

            { 51, 0 , 20 },
            { 52, 0 , 20 },
            { 53, 0 , 20 },
            { 54, 0 , 20 },
            { 55, 0 , 20 },
            { 56, 0 , 20 },
            { 57, 0 , 20 },
            { 58, 0 , 20 },
            { 59, 0 , 20 },
            { 60, 0 , 20 },

            { 61, 0 , 20 },
            { 62, 0 , 20 },
            { 63, 0 , 20 },
            { 64, 0 , 20 },
            { 65, 0 , 20 },
            { 66, 0 , 20 },
            { 67, 0 , 20 },
            { 68, 0 , 20 },
            { 69, 0 , 20 },
            { 70, 0 , 20 },

            { 71, 0 , 20 },
            { 72, 0 , 20 },
            { 73, 0 , 20 },
            { 74, 0 , 20 },
            { 75, 0 , 20 },
            { 76, 0 , 20 },
            { 77, 0 , 20 },
            { 78, 0 , 20 },
            { 79, 0 , 20 },
            { 80, 0 , 20 },

            { 81, 0 , 20 },
            { 82, 0 , 20 },
            { 83, 0 , 20 },
            { 84, 0 , 20 },
            { 85, 0 , 20 },
            { 86, 0 , 20 },
            { 87, 0 , 20 },
            { 88, 0 , 20 },
            { 89, 0 , 20 },
            { 90, 0 , 20 },

            { 91, 0 , 20 },
            { 92, 0 , 20 },
            { 93, 0 , 20 },
            { 94, 0 , 20 },
            { 95, 0 , 20 },
            { 96, 0 , 20 },
            { 97, 0 , 20 },
            { 98, 0 , 20 },
            { 99, 0 , 20 },
        };


        public Home()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

            DataColumn KmColuns = table.Columns.Add("Kilometers", typeof(short) );
            KmColuns.ReadOnly = true;
            table.Columns.Add("Gain Setting\n\nReal Gain =\nGain Setting * 1/1000", typeof(short));
            table.Columns.Add("Freq Shift Nominator Setting\n\nReal Freq Shift =\nNominator / Denominator", typeof(short));
            
            for (int i = 0 ; i < km; i++)
            {
                table.Rows.Add(
                    default_table_settings[i, 0], 
                    default_table_settings[i, 1], 
                    default_table_settings[i, 2]
                    );
            }
            
            dataGridView1.DataSource = table;
   
            btn_open.Enabled = false;
            btn_generate.Enabled = false;

            lbl_version.Text = "Tool Version: "+ calibration_tool_version;

            ShiftDenominator = BitConverter.GetBytes(Convert.ToInt16(txt_boxShiftDenominator.Text));
        }

        private void btn_open_Click(object sender, EventArgs e)
        {
            try
            {
                using (OpenFileDialog dlg = new OpenFileDialog() { Filter = "WAV|*.wav", Multiselect = false, ValidateNames = true })
                {
                    if (dlg.ShowDialog() == DialogResult.OK)
                    {
                        txt_filePath.Text = dlg.FileName;
                        txt_filePath.Text = txt_filePath.Text.Replace("\\", "\\\\");
                    }

                }
                wavFile = Functions.ReadWavFile(txt_filePath.Text);
                if (wavFile == null )
                {
                    MessageBox.Show("Error", "File Opened Unsuccessfully", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    txt_filePath.BackColor = Color.Red;
                }
                else
                {
                    MessageBox.Show("File Opened Successfully", " Successful", MessageBoxButtons.OK, MessageBoxIcon.Information);
                    txt_filePath.BackColor = Color.Green;
                    btn_generate.Enabled = true;
                    audioFileSize = wavFile.Length;

                    byte[] wavFormatBytes = Functions.ReadByte(txt_filePath.Text, 20, 2);
                    var wavFormatValue = (wavFormatBytes[0]) + (wavFormatBytes[1] << 8);
                    txt_wavFormat.Text = wavFormatValue.ToString();

                    byte[] numChannelsBytes = Functions.ReadByte(txt_filePath.Text, 22, 2);
                    var numChannelsValue = (numChannelsBytes[0] ) + (numChannelsBytes[1] << 8);
                    txt_numChannels.Text = numChannelsValue.ToString();


                    byte[] sampleRateBytes = Functions.ReadByte(txt_filePath.Text, 24, 4);
                    var sampleRateValue = (sampleRateBytes[0]) + (sampleRateBytes[1] << 8) + (sampleRateBytes[2] << 16) + (sampleRateBytes[3] << 24);
                    txt_sampleRate.Text = sampleRateValue.ToString();

                    byte[] bitsPerSampleBytes = Functions.ReadByte(txt_filePath.Text, 34, 2);
                    var bitsPerSampleValue = (bitsPerSampleBytes[0]) + (bitsPerSampleBytes[1] << 8);
                    txt_bitsPerSample.Text = bitsPerSampleValue.ToString();

                    byte[] byteRateBytes = Functions.ReadByte(txt_filePath.Text, 28, 4);
                    var byteRateValue = (byteRateBytes[0]) + (byteRateBytes[1] << 8) + (byteRateBytes[2] << 16) + (byteRateBytes[3] << 24);
                    txt_byteRate.Text = byteRateValue.ToString();
                }
            }
            catch
            {
                MessageBox.Show("Error", "File Opened Unsuccessfully", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

          


        }

        private void helpToolStripMenuItem_Click(object sender, EventArgs e)
        {
            MessageBox.Show("PC Tool for Audio File Generation\r\n\r\nIntroduction\r\n\r\nThe AVS2022003 project requires the use of a PC tool to generate audio files that contain both raw audio data and Daiichi metadata. These audio files will be used for calibrated audio playing on the AVAS External Flash.\r\n\r\n\r\nPC Tool Usage\r\n\r\nTo generate the necessary audio files, follow the steps below:\r\n\r\n\r\n2.1. Launch the PC Tool\r\n\r\nDouble-click on the PC Tool icon to launch the application.\r\n\r\n\r\n2.2. Input Data\r\n\r\nOnce the PC Tool is open, the calibrator person needs to input the required data. This data may include specific audio parameters, metadata information, or any other relevant details for calibration.\r\n\r\n\r\n2.3. Generate Audio Files\r\n\r\nAfter inputting the necessary data, the calibrator person can initiate the process of generating the audio files. The PC Tool will process the provided data and combine it with the raw audio data to create a single binary file.\r\n\r\n\r\n2.4. Merged Binary File\r\n\r\nOnce the generation process is complete, the PC Tool will produce a merged binary file. This file contains the calibrated audio data along with the associated Daiichi metadata.\r\n\r\n\r\nConclusion\r\n\r\nBy following the steps outlined in this user guide, users can effectively utilize the PC Tool to generate audio files containing raw audio data and Daiichi metadata for the AVS2022003 project.\r\n\r\n\r\n\r\nContact information ;\r\n\r\ninfo@daiichi.com", "Help", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
        {

            
            MessageBox.Show("Application version information: " + calibration_tool_version + "\r\n\r\nCompany information: Daiichi Infotainment Systems", "About", MessageBoxButtons.OK, MessageBoxIcon.Information);
        }

        private void btn_generate_Click(object sender, EventArgs e)
        {
           List<byte[]> liste = Functions.ConvertDataGridViewToByteList(dataGridView1 , 0 , 600);

           string fileName = "Daiichi_Avas_Audio_File_" + txtBox_CalibrationVersion.Text + ".bin";
           try
            {

                DateTime now = DateTime.Now;
                string formattedDateTime = now.ToString("dd.MM.yyyy-HH:mm");


                byte[] data = BitConverter.GetBytes(audioFileSize);
                List<byte> dataList = new List<byte>(data);
                dataList.Reverse();
                byte[] reversedData = dataList.ToArray();

                audioFileName = Path.GetFileName(txt_filePath.Text);
                Functions.WriteByte(fileName, 0x0000, Encoding.ASCII.GetBytes(audioFileName));
                Functions.WriteByte(fileName, 0x0020, reversedData);
                Functions.WriteByte(fileName, 0x0024, Encoding.ASCII.GetBytes(txtBox_AudioFileVersion.Text));
                Functions.WriteByte(fileName, 0x0028, Encoding.ASCII.GetBytes(txtBox_CalibrationVersion.Text));
                Functions.WriteByte(fileName, 0x002C, Encoding.ASCII.GetBytes(calibration_tool_version.ToString()));
                Functions.WriteByte(fileName, 0x0030, Encoding.ASCII.GetBytes(formattedDateTime));
                Functions.WriteByte(fileName, 0x0040, ShiftDenominator);
                Functions.WriteByte(fileName, 0x0042, reservedData);

                foreach (var item in liste)
                {
                    Array.Reverse(item);
                }

                Functions.WriteList(fileName, 0x0064, liste);
                Functions.WriteByte(fileName, 0x01F4, wavFile);

                Functions.SplitFileIntoChunks(fileName, 40960);

                MessageBox.Show("File Generation Successfully", " Successful", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }

            
            catch {

                MessageBox.Show("File Generation Unsuccessfully", "Fail" , MessageBoxButtons.OK, MessageBoxIcon.Error);

            }
        }

        private void txtBox_AudioFileVersion_TextChanged(object sender, EventArgs e)
        {
            if (System.Text.Encoding.UTF8.GetByteCount(txtBox_AudioFileVersion.Text) == 4 && txtBox_AudioFileVersion.Text.IndexOf(".") == 2)
            {
                txtBox_AudioFileVersion.BackColor = Color.Green;
                if (txt_boxShiftDenominator.BackColor == Color.Green &&  txtBox_CalibrationVersion.BackColor == Color.Green)
                {
                    btn_open.Enabled = true;
                }
            }
            else
            {
                txtBox_AudioFileVersion.BackColor = Color.Red;
                btn_open.Enabled = false;
                btn_generate.Enabled = false;
            }
              
        }

        private void txtBox_CalibrationVersion_TextChanged(object sender, EventArgs e) 
        {
            if (System.Text.Encoding.UTF8.GetByteCount(txtBox_CalibrationVersion.Text) == 4 && txtBox_CalibrationVersion.Text.IndexOf(".") == 2)
            {   
                txtBox_CalibrationVersion.BackColor = Color.Green;

                if(txt_boxShiftDenominator.BackColor == Color.Green && txtBox_AudioFileVersion.BackColor == Color.Green )
                {
                    btn_open.Enabled = true;
                }

            }
            else
            {
                txtBox_CalibrationVersion.BackColor = Color.Red;
                btn_open.Enabled = false;
                btn_generate.Enabled = false;
            }
        }
       
        
        private void txt_boxShiftDenominator_TextChanged(object sender, EventArgs e)
        {
            try
            {
                Convert.ToUInt16(txt_boxShiftDenominator.Text);
                txt_boxShiftDenominator.BackColor = Color.Green;

                if (txt_boxShiftDenominator.BackColor == Color.Green && txtBox_CalibrationVersion.BackColor == Color.Green && txtBox_AudioFileVersion.BackColor== Color.Green )
                {
                    btn_open.Enabled = true;
                }

                if (txt_boxShiftDenominator.Text != null)
                {
                    ShiftDenominator = BitConverter.GetBytes(Convert.ToInt16(txt_boxShiftDenominator.Text));
                    Array.Reverse(ShiftDenominator);
                }

            }
            catch
            {
                txt_boxShiftDenominator.BackColor = Color.Red;
                btn_open.Enabled = false;
                btn_generate.Enabled = false;
            }
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e)
        {
            openToolStripMenuItem_Click(sender, e, table);
        }

        private void openToolStripMenuItem_Click(object sender, EventArgs e, DataTable table)
        {
          
            using (OpenFileDialog dlg = new OpenFileDialog() { Filter = "BIN Files (*.bin)|*.bin", Multiselect = false, ValidateNames = true })
            {
                if (dlg.ShowDialog() == DialogResult.OK)
                {
                    txt_binPath.Text = dlg.FileName;
                    txt_binPath.Text = txt_binPath.Text.Replace("\\", "\\\\");
                }

            }

            try
            {

                groupBox3.Visible = true;
                lbl_audioFileName.Text = System.Text.Encoding.Default.GetString(Functions.ReadByte(txt_binPath.Text, 0x0000, 32));
                txtBox_AudioFileVersion.Text = System.Text.Encoding.Default.GetString(Functions.ReadByte(txt_binPath.Text, 0x0024, 4));
                txtBox_CalibrationVersion.Text = System.Text.Encoding.Default.GetString(Functions.ReadByte(txt_binPath.Text, 0x0028, 4));
                lbl_FileToolVersion.Text = System.Text.Encoding.Default.GetString(Functions.ReadByte(txt_binPath.Text, 0x002C, 4));



                byte[] data = Functions.ReadByte(txt_binPath.Text, 0x0040, 2);
                List<byte> dataList = new List<byte>(data);
                dataList.Reverse(); //reverse'e gerek yok
                byte[] reversedData = dataList.ToArray();

                txt_boxShiftDenominator.Text = BitConverter.ToInt16(reversedData, 0).ToString();

                table.Clear();

                short[] gain = new short[km];
                short[] freq = new short[km];

               

                gain = Functions.ReadShortArray(txt_binPath.Text, 0x0064, km);
                freq = Functions.ReadShortArray(txt_binPath.Text, 0x012C, km);

                

                for (int i = 0; i < km; i++)
                {
                    table.Rows.Add(i, gain[i], freq[i]);
                }

                MessageBox.Show("File Opened Successfully", " Successful", MessageBoxButtons.OK, MessageBoxIcon.Information);
            }
            catch
            {
                MessageBox.Show("File Opened Unsuccessfully", "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

        }

        private void exitToolStripMenuItem_Click(object sender, EventArgs e)
        {   
            this.Close();
            System.Windows.Forms.Application.Exit();
        }

        private void clearToolStripMenuItem_Click(object sender, EventArgs e)
        {
            System.Windows.Forms.Application.Restart();
        }
    }
}

public class Functions
{
    public static byte[] ReadWavFile(string filePath)
    {
        using (FileStream fs = File.OpenRead(filePath))
        {
            using (BinaryReader reader = new BinaryReader(fs))
            {
                // Read WAV file header
                byte[] header = reader.ReadBytes(44);

                // Get audio data size
                int dataSize = BitConverter.ToInt32(header, 40);

                // Read audio data
                byte[] audioData = reader.ReadBytes(dataSize);
                
                return audioData;
            }
        }
    }

    public static void WriteByte(string fileName, long address, byte[] buffer)
    {

        using (BinaryWriter binWriter =
           new BinaryWriter(File.Open(fileName, FileMode.OpenOrCreate)))
        {

            binWriter.BaseStream.Seek(address, SeekOrigin.Begin);
            binWriter.Write(buffer);
            binWriter.Close();
        }

        Console.WriteLine("Bit pattern written successfully.");
    }

    public static void WriteList(string fileName, long address, List<byte[]> buffer)
    {

        using (BinaryWriter binWriter =
           new BinaryWriter(File.Open(fileName, FileMode.OpenOrCreate)))
        {

            binWriter.BaseStream.Seek(address, SeekOrigin.Begin);
            
            foreach (byte[] item in buffer)    
            {
                binWriter.Write(item); 
            }
            binWriter.Close();
        }

        Console.WriteLine("Bit pattern written successfully.");
    }

    public static List<byte[]> ConvertDataGridViewToByteList(DataGridView dataGridView , int min , int max )
    {
        List<byte[]> liste = new List<byte[]>();

        for (int column = 1; column <= 2; column++)
        {
            for (int row = 0; row < dataGridView.Rows.Count; row++)
            {
                object cellValue = dataGridView.Rows[row].Cells[column].Value;

                if (cellValue != null /*&& Convert.ToInt16(cellValue) > min  && Convert.ToInt16(cellValue) < max */)
                {
                    byte[] data = BitConverter.GetBytes((short)cellValue);
                    liste.Add(data);
                }
               /* else
                {
                    MessageBox.Show("Column no:" + column.ToString() + " Row no: " + row.ToString() + " Value invalid ", " Error ", MessageBoxButtons.OK, MessageBoxIcon.Error);
                    
                }*/
                
            }
        }

        return liste;
    }

    public static byte[] ReadByte(string fileName, long address, int size)
    {
        using (BinaryReader reader = new BinaryReader(File.Open(fileName, FileMode.Open)))
        {

            byte[] okunanVeri;
            reader.BaseStream.Seek(address, SeekOrigin.Begin);
            okunanVeri = reader.ReadBytes(size);

            reader.Close();
            return okunanVeri;
        }
    }

    public static short[] ReadShortArray(string filePath, long startPosition, int length )
    {
        short[] buffer = new short[length];

        using (BinaryReader reader = new BinaryReader(File.Open(filePath, FileMode.Open)))
        {

            reader.BaseStream.Seek(startPosition, SeekOrigin.Begin);
    
            for (int i = 0; i < length; i++)
            {


                byte[] data = reader.ReadBytes(2);
                List<byte> dataList = new List<byte>(data);
                dataList.Reverse();
                byte[] reversedData = dataList.ToArray();

                short value = BitConverter.ToInt16(reversedData, 0);
                buffer[i] = value;
            }
        }

        return buffer;
    }

    public static void SplitFileIntoChunks(string filePath, int chunkSize)
    {
        if (!File.Exists(filePath))
        {
            Console.WriteLine("Dosya bulunamadı.");
            return;
        }

        using (var fileStream = new FileStream(filePath, FileMode.Open, FileAccess.Read))
        {
            byte[] buffer = new byte[chunkSize];
            int bytesRead;

            int partNumber = 1;

            while ((bytesRead = fileStream.Read(buffer, 0, buffer.Length)) > 0)
            {
                string partFileName = Path.GetFileNameWithoutExtension(filePath) + "_part" + partNumber + ".bin";
                string partFilePath = Path.Combine(Path.GetDirectoryName(filePath), partFileName);

                using (var partFileStream = new FileStream(partFilePath, FileMode.Create, FileAccess.Write))
                {
                    partFileStream.Write(buffer, 0, bytesRead);
                }

                partNumber++;
            }
        }

        Console.WriteLine("Dosya parçalara ayrılarak kaydedildi.");
    }
}
