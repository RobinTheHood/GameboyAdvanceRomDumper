/*
 * (C) Copyright 2016 Robin Wieschendorf
 *
 * http://www.robinwieschendorf.de
 * mail@robinwieschendorf.de
 *
 */

package gbaromdumper.java;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class RomWriter { 
    private FileOutputStream writer;
    private int byteCount = 0;
    private int zeroCount = 0;
    private int lastData = 0;
    
    public int getByteCount()
    {
        return byteCount;
    }
    
    public int getZeroCount()
    {
        return zeroCount;
    }
    
    public void open(String absolutePath)
    {
        File file = new File(absolutePath);
        try {
            writer = new FileOutputStream(file);
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
    
    public void writeByte(int c)
    {
        if (c == lastData) {
            zeroCount++;
        } else {
            zeroCount = 0;
            lastData = c;
        }
        
        try {
            byteCount++;
            writer.write(c);
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
    
    public void close()
    {
        try {
            writer.flush();
            writer.close();
        } catch (IOException ex) {
            ex.printStackTrace();
        }
    }
}
