
import com.sun.webkit.ThemeClient;
import java.awt.image.BufferedImage;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.rmi.NotBoundException;
import java.rmi.RemoteException;
import java.rmi.registry.LocateRegistry;
import java.rmi.registry.Registry;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.imageio.ImageIO;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 *
 * @author Peni Sriwahyu
 */
public class threadClDua extends Thread{
    String ipServer;
    int id;
    int n;
    
    public threadClDua (String ipServer, int id, int n){
        this.ipServer = ipServer;
        this.id=id;
        this.n=n;
    }
    
    @Override
    public void run(){
        
    }
    
    
}
