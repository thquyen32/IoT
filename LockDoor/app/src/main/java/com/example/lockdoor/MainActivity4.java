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
public class MainActivity4 extends AppCompatActivity {
    EditText change,confirm;
    Button changepass;
    TextView noti;
    String urlServer = "http://192.168.1.10:8483/changepass";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main4);
        change = findViewById(R.id.change);
        confirm = findViewById(R.id.confirm);
        changepass = findViewById(R.id.button);
        noti = findViewById(R.id.noti);
        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        changepass.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String a = confirm.getText().toString();
                new Thread(new Runnable() {
                    @Override
                    public void run() {
                        RequestBody postBody = new FormBody.Builder()
                                .add("PASS", a)
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
                        } catch (IOException e) {
                            e.printStackTrace();
                        }
                        }
                }).start();
            }
        });
        noti.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Log.d("DEBUG", "Noti clicked");
                Intent intent = new Intent(MainActivity4.this, MainActivity.class);
                startActivity(intent);
            }
        });
    }
}