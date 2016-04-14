/*
 * (C) Copyright 2016 Robin Wieschendorf
 *
 * http://www.robinwieschendorf.de
 * mail@robinwieschendorf.de
 *
 */

package gbaromdumper.java;

public class TextInterface {
    public final static int ESC_CODE = 0x1B;
    
    private int totalBytes;
    private int bytesPerSec;
    
    public void setTotalBytes(int totalBytes)
    {
        this.totalBytes = totalBytes;
    }
    
    public void setBytesPerSec(int bytesPerSec)
    {
        this.bytesPerSec = bytesPerSec;
    }
    
    public void drawInterface()
    {
        clear();
        
        System.out.println("Dumping GBA ROM ...");
        drawBar();
        drawInfo();
    }
    
    private void clear()
    {   
        System.out.print(String.format("%c[%dJ", ESC_CODE, 1)); // clear screen
        System.out.print(String.format("%c[%d;%df", ESC_CODE, 0, 0)); // set coursor to 0, 0;
    }
    
    private void drawBar()
    {
        float megaBytes = totalBytes / 1000000f;
        int times = map(megaBytes, 0, 32, 0, 78);
        
        String barFilled = repeatString("#", times);
        String barUnfilled = repeatString("_", 78 - times);
        
        System.out.println("0 MB                                                                       32 MB");
        System.out.println("[" + barFilled + barUnfilled  + "]");
    }
    
    private void drawInfo()
    {
        float megaBytes = totalBytes / 1000000f;
        float kiloBytesPerSec = bytesPerSec / 1000f;
        
        //System.out.println("Total: " + megaBytes + " MB, Rate: " + kiloBytesPerSec + " KB/s");
        System.out.println("Total: " + megaBytes + " MB");
    }
    
    private int map(float value, int minIn, int maxIn, int minOut, int maxOut)
    {
        int diffIn = maxIn - minIn;
        int diffOut = maxOut - minOut;
        float factor =  value / (float) diffIn;
        return Math.round(minOut + diffOut * factor);
    }
    
    private String repeatString(String str, int times)
    {
        String result = "";
        for(int i=0; i<times; i++) {
            result += str;
        }
        return result;
    }
}
