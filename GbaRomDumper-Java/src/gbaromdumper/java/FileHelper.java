/*
 * (C) Copyright 2016 Robin Wieschendorf
 *
 * http://www.robinwieschendorf.de
 * mail@robinwieschendorf.de
 *
 */

package gbaromdumper.java;

import java.io.File;

public class FileHelper {
    
    public static String getProgrammAbsoluteDir()
    {
        String programmAbsoluteDir = "";
        String path = System.getProperty("java.class.path");
        String extension = "";
        int i = path.lastIndexOf('.');
        if (i > 0) {
            extension = path.substring(i+1);
        }
        
//        if (extension.equals("jar")) {
//            i = path.lastIndexOf(File.separator);
//            programmAbsoluteDir = path.substring(0, i);
//        } else {
//            programmAbsoluteDir = new File("").getAbsolutePath();
//        }
        
        programmAbsoluteDir = new File("").getAbsolutePath();
        
        return programmAbsoluteDir;
    }
    
    public static String toOSPath(String path)
    {
        String result = "";
        for (int i = 0; i < path.length(); i++) {
            char c = path.charAt(i);
            if (c == '/') {
                c = File.separatorChar;
            }
            result += c;
        }
        return result;
    }
    
    public static String toAbsoluteOSPath(String relativPath)
    {
        return getProgrammAbsoluteDir() + toOSPath(relativPath);
    }
    
    public static String getFileExtention(String path)
    {
        String[] splitArray = path.split("\\.");
        String ext = splitArray[splitArray.length -1];
        return ext;
    }
}
