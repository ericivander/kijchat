
import java.io.DataInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JList;
import javax.swing.JTextArea;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 *
 * @author Peni Sriwahyu
 */
class threadReadClient extends Thread{
    private Socket sock;
    private DataInputStream is;
    private JTextArea txtReceived;
    private JList list;
    private ClientForm client;
    private String request;
    
    public threadReadClient(ClientForm parent, Socket sock, DataInputStream is, JTextArea txtReceived, JList list){
        this.sock = sock;
        this.is = is;
        this.txtReceived = txtReceived;
        this.list = list;
        
    }
    
    @Override
    public void run(){
        while(true){
            try {
                String message = is.readLine();
                System.out.println("Message: " + message);
            } catch (IOException ex) {
                Logger.getLogger(threadReadClient.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
}
