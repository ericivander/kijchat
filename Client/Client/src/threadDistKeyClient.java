
import java.io.DataInputStream;
import java.io.IOException;
import java.net.Socket;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.DefaultListModel;

/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */
/**
 *
 * @author Admin
 */
public class threadDistKeyClient extends Thread {

    private Socket sock;
    private DataInputStream is;
    private ClientForm client;
    private String respon;
    private DefaultListModel listModel;

    public threadDistKeyClient(ClientForm parent, Socket sock, DataInputStream is) {
        this.sock = sock;
        this.is = is;
        this.client = parent;
    }

    @Override
    public void run() {
        boolean flag = true;
        while (client.isConnected) {
            try {
                respon = is.readLine();
                String[] parts = respon.split("#");

                // server-key 110 Username Set
                if (parts[0].equals("110")) {
                    //JOptionPane.showMessageDialog(null, "SET " + this.myKey.getPublicKey());
                    this.client.send_to_key("SET " + this.client.myKey.getPublicEKey() + " " + this.client.myKey.getPublicNKey());
                } // server-key 410 public key of client set
                else if (parts[0].equals("410")) {
                    //JOptionPane.showMessageDialog(null, "Public Key Terkirim");
                } // server-key 411 public key of target get
                else if (parts[0].equals("411")) {
                    //JOptionPane.showMessageDialog(null, "Public Key " + parts[1] + " Telah Diterima");
                    //System.out.println(parts[2]);
                    //System.out.println(parts[3]);
                    this.client.setPubEKey(parts[2]);
                    this.client.setPubNKey(parts[3]);
                } else {
                    System.out.println(respon);
                }
            } catch (IOException ex) {
                Logger.getLogger(threadReadClient.class.getName()).log(Level.SEVERE, null, ex);
            }
        }
    }
}
