
import java.io.DataInputStream;
import java.io.IOException;
import java.io.ObjectInputStream;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.DefaultListModel;
import javax.swing.JComboBox;
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
    private JTextArea list;
    private ClientForm client;
    private String respon;
    private JComboBox room;
    private DefaultListModel listModel;
    
    public threadReadClient(ClientForm parent, Socket sock, DataInputStream is, JTextArea txtReceived, JTextArea list, JComboBox room){
        this.sock = sock;
        this.is = is;
        this.txtReceived = txtReceived;
        this.list = list;
        this.room = room;
    }
    
    @Override
    public void run(){
        boolean flag = true;
        boolean chat = true; 
        while(true){
            try {
                respon = is.readLine();
                String[] parts = respon.split(" ");
                if(parts[0].equals("LIST")){
                    String[] listUser = parts[1].split(";");
                    for(int i=0; i<listUser.length; i++){
                        this.room.addItem(listUser[i]);
                        this.list.append(listUser[i] + "\n");
                    }
                    
                }
                else if(parts[0].equals("MSG")){
                    
                }
                else{
                    System.out.println(respon);
                }
            } catch (IOException ex) {
                Logger.getLogger(threadReadClient.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
}
