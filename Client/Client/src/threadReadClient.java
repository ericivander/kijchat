
import com.sun.org.apache.xerces.internal.impl.dv.util.Base64;
import java.io.DataInputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.DefaultListModel;
import javax.swing.JComboBox;
import javax.swing.JOptionPane;
import javax.swing.JTextArea;
import javax.swing.JTextField;

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
    private JTextField uname;
    private DefaultListModel listModel;
	private byte[] DESKey = new byte[8];
    private Random r = new Random();
    private int bitLength = 64;
    private RSA myKey = new RSA();
    
    public threadReadClient(ClientForm parent, Socket sock, DataInputStream is, JTextArea txtReceived, JTextArea list, JComboBox room, JTextField uname){
        this.sock = sock;
        this.is = is;
        this.txtReceived = txtReceived;
        this.list = list;
        this.room = room;
        this.client = parent;
        this.uname = uname;
		r.nextBytes(DESKey);
        System.out.println("DES KEY : " + bytesToHex(DESKey));
    }
    
    @Override
    public void run(){
        boolean flag = true;
        while(client.isConnected){
            try {
                respon = is.readLine();
                String[] parts = respon.split("#");
                
				// server 400 LIST Response
                if(parts[0].equals("400")){
                    this.room.removeAllItems();
                    this.list.setText("");
                    String[] listUser = parts[1].split(";");
                    for(int i=0; i<listUser.length; i++){
                        if (!(this.uname.getText().equals(listUser[i]))){
                            this.room.addItem(listUser[i]);
                            this.list.append(listUser[i] + "\n");
                        }
                    }
                }
				// server 401 MSG Response
                else if(parts[0].equals("401")){
                    try {
                        byte[] theKey = "hehehehe".getBytes();
                        byte[] IV = "hohohoho".getBytes();
                        byte[] theMsg = Base64.decode(parts[2]);
                        byte[][] subKeys = DES.DES.getSubkeys(theKey);
                        byte[] chiper = DES.DES.paddingMsg(theMsg);
                        byte[] plain = DES.DES.encryptBlock(chiper, IV, subKeys);
                        //System.out.println(DES.DES.bytesToHex(plain));
                        //System.out.println("lalala");
                        String text = parts[1] + " : " + new String(plain);
                        this.txtReceived.append(text + "\n");
                        //System.out.println(new String(plain));
                    }catch (Exception e) {
                        e.printStackTrace();
                        return;
                    }
                }
				// server 102 Username Already Taken
                else if(parts[0].equals("102")){
                    JOptionPane.showMessageDialog(null, "Username Sudah Terpakai");
                }
				// server 100 Username Set
                else if(parts[0].equals("100")){
                    this.client.send("LIST");
                    this.client.setConBtn(true);
                    this.client.setEnObject(true);
                    this.client.setEdit(false);
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
