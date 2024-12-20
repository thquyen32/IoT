package com.example.lockdoor;

import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import androidx.appcompat.app.AppCompatActivity;
import android.util.Log;
import android.widget.TextView;

import java.io.IOException;
import okhttp3.Call;
import okhttp3.FormBody;
import okhttp3.OkHttpClient;
import okhttp3.Request;
import okhttp3.RequestBody;
import okhttp3.Response;

public class MainActivity extends AppCompatActivity {

    Button button;
    EditText SSID, PASS;
    TextView state;
    String urlServer = "http://192.168.1.10:8483/";  // Địa chỉ server của bạn
    String serverResponse = "";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        SSID = findViewById(R.id.change);
        PASS = findViewById(R.id.confirm);
        button = findViewById(R.id.button);
        state = findViewById(R.id.state);

        button.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Lấy giá trị từ EditText
                String ssid = SSID.getText().toString();
                String pass = PASS.getText().toString();
                Intent intent = new Intent(MainActivity.this, MainActivity2.class);
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        RequestBody postBody = new FormBody.Builder()
                                .add("SSID",ssid)
                                .add("PASS",pass)
                                .build();

                        Request request = new Request.Builder()
                                .url(urlServer)
                                .post(postBody)
                                .build();
                        OkHttpClient client = new OkHttpClient();
                        Call call = client.newCall(request);

                        Response response = null;
                        try {
                            response = call.execute();
                            serverResponse = response.body().string();
                            runOnUiThread(() -> state.setText(serverResponse));
                            Log.d("Server response",serverResponse);
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                    }
                }).start();
                Log.d("Server response2",serverResponse);
                if(serverResponse.equals("Accept"))
                {
                    startActivity(intent);
                }

            }
        });
    }

}
