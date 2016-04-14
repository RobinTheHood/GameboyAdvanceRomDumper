/*
 * (C) Copyright 2016 Robin Wieschendorf
 *
 * http://www.robinwieschendorf.de
 * mail@robinwieschendorf.de
 *
 */

package gbaromdumper.java;

import com.fazecast.jSerialComm.SerialPort;
import java.util.Scanner;

public class GbaRomDumper {
    public final static int BAUD_RATE = 230400;
    
    private String romTitle;
    private int portIndex;
    
    private String token = "";
    private boolean startFlag = false;
    
    public static void main(String[] args)
    {
        GbaRomDumper gbaRomDumper = new GbaRomDumper();
        Scanner scanner = new Scanner(System.in);
        
        System.out.println("Arduino GBA Rom Dumper (Version 0.9)");
        System.out.println("Enter name of rom/dump: ");
        String romTitle = scanner.nextLine();
        System.out.println("");
        
        System.out.println("Select serial port");
        SerialPort[] serialPorts = SerialPort.getCommPorts();
        int portNumber = 1;
        for (SerialPort serialPort : serialPorts) {
            System.out.println(portNumber++ + ". " + serialPort.getSystemPortName());
        }
        System.out.println("Enter number: ");
        int portIndex = Integer.parseInt(scanner.nextLine()) - 1;
        System.out.println("");
        
        gbaRomDumper.setRomTitle("/" + romTitle + ".gba");
        gbaRomDumper.setPortIndex(portIndex);
        gbaRomDumper.dump();
    }
 
    public void setRomTitle(String romTitle)
    {
        this.romTitle = romTitle;
    }
    
    public void setPortIndex(int portIndex)
    {
        this.portIndex = portIndex;
    }
    
    public void dump()
    {
        TextInterface textInterface = new TextInterface();
        
        SerialPort[] serialPorts = SerialPort.getCommPorts();
        SerialPort serialPort = serialPorts[this.portIndex];
       
        if (serialPort.openPort()) {
            System.out.println("Serial port sucessful opened.");
        } else {
            System.out.println("Open serial port failed. exit");
            System.exit(-1);
        }
        
        serialPort.setBaudRate(BAUD_RATE);
        serialPort.setComPortTimeouts(SerialPort.TIMEOUT_READ_SEMI_BLOCKING, 1000, 0);
        
        RomWriter romWriter = new RomWriter();
        romWriter.open(FileHelper.toAbsoluteOSPath(this.romTitle));
        
        System.out.println("Waiting for start token ...");
        
        while(true) {
           byte[] readBuffer = new byte[1024];
           int numRead = serialPort.readBytes(readBuffer, readBuffer.length);

            for (int i=0; i<numRead; i++) {
                if (startFlag == false) {
                    startFlag = checkStartFlag(readBuffer[i]);
                } else {
                    romWriter.writeByte(readBuffer[i]);
                    
                    int byteCount = romWriter.getByteCount();
                    if (byteCount % 10000 == 0) {
                        textInterface.setTotalBytes(byteCount);
                        textInterface.drawInterface();
                    }
                    
                    
                    if (romWriter.getZeroCount() > 100000) {
                        //System.out.println("NO MORE CHANGING DATA - DONE");
                        //romWriter.close();
                        //System.exit(0);
                    }
                }
            }
        }
    }
    
    boolean checkStartFlag(byte value)
    {
        String startToken = "***START DUMP***";
        
        token += (char) value;
        if (token.length() > startToken.length()) {
            token = token.substring(1);
        }
        
        if (token.equals(startToken)) {
            return true;
        }
        return false;
    }
}
