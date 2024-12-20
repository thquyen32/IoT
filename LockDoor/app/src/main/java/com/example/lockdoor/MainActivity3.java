package com.example.lockdoor;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;
import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;
import java.io.IOException;
import okhttp3.Call;
import okhttp3.FormBody;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;



public class MainActivity3 extends AppCompatActivity {
    EditText SSID,PASS;
    TextView noti;
    Button Verify;
    String urlServer = "http://192.168.1.10:8483/";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main3);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        SSID = findViewById(R.id.change);
        PASS = findViewById(R.id.confirm);
        noti = findViewById(R.id.noti);
        Verify = findViewById(R.id.button);
        Intent intent = new Intent(MainActivity3.this,MainActivity4.class);
        Verify.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
               String a =  SSID.getText().toString();
               String b = PASS.getText().toString();
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        RequestBody postBody = new FormBody.Builder()
                                .add("SSID", a)
                                .add("PASS",b)
                                .build();

                        Request request = new Request.Builder()
                                .url(urlServer)
                                .post(postBody)
                                .build();

                        OkHttpClient client1 = new OkHttpClient();
                        Call call =client1.newCall(request);

                        Response response = null;
                        try {
                            response = call.execute();
                            String serverResponse = response.body().string();
                            Log.d("Reponse",serverResponse);
                            if(serverResponse.equals("Accept"))
                            {
                            startActivity(intent);
                            }
                            else {
                                runOnUiThread(new Runnable() {
                                    @Override
                                    public void run() {
                                        noti.setText("Your SSID and PASSWORD are INVALID");
                                    }
                                });
                            }
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
            }
        });
    }
}