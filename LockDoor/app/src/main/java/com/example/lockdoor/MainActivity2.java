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
import okhttp3.WebSocket;
import okhttp3.WebSocketListener;

import org.json.JSONException;
import org.json.JSONObject;


public class MainActivity2 extends AppCompatActivity {
    private OkHttpClient client;
    private WebSocket webSocket;
    TextView state,forgot;
    EditText password;
    Button open;
    String urlServer = "http://192.168.1.10:8483/android    ";
    String serverResponse="";

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        setContentView(R.layout.activity_main2);
        state = findViewById(R.id.state);
        password = findViewById(R.id.password);
        open = findViewById(R.id.open);
        forgot = findViewById(R.id.forgot);

        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.main), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });
        start1();
        forgot = findViewById(R.id.forgot);
        open.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String a = password.getText().toString();
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
                        runOnUiThread(new Runnable() {
                            @Override
                            public void run() {
                                state.setText(serverResponse);
                            }
                        });
                    } catch (IOException e) {
                        e.printStackTrace();
                    }
                }
            }).start();
            }
        });
        forgot.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity2.this, MainActivity3.class);
                startActivity(intent);
            }
        });
    }
    public void start1() {
        // Kiểm tra nếu client chưa khởi tạo, sẽ khởi tạo lại
        if (client == null) {
            client = new OkHttpClient();
        }

        // Kiểm tra nếu webSocket đã tồn tại và chưa bị đóng trước khi khởi tạo
        if (webSocket == null) {
            Request request = new Request.Builder().url("http://192.168.1.10:8483/").build();
            EchoWebSocketListener webSocketListener = new EchoWebSocketListener();
            webSocket = client.newWebSocket(request, webSocketListener);  // Tạo mới webSocket
            ID_WEBSOCKET();
        } else {
            Log.d("WebSocket", "WebSocket already open");
        }
    }
    public void ID_WEBSOCKET() {
        try {
            JSONObject json = new JSONObject();
            json.put("type", "Register");
            json.put("ID", "2");
            String jsonString = json.toString();
            webSocket.send(jsonString);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        if (webSocket != null) {
            webSocket.close(1000, "App closed");
        }
        if (client != null) {
            client.dispatcher().executorService().shutdown();
        }
    }

    private final class EchoWebSocketListener extends WebSocketListener {
        @Override
        public void onOpen(WebSocket webSocket, Response response) {
            Log.d("webSocket","Connected");
        }

        @Override
        public void onMessage(WebSocket webSocket, String text) {
            Log.d("webSocket",text);
            runOnUiThread(() -> state.setText(text));
        }

        @Override
        public void onClosed(WebSocket webSocket, int code, String reason) {
            Log.d("WebSocket", "Closed: " + code + " / " + reason);
        }

        @Override
        public void onFailure(WebSocket webSocket, Throwable t, Response response) {
            Log.e("WebSocket", "Failure: " + t.getMessage());
            // Thử kết nối lại sau một khoảng thời gian
            new android.os.Handler().postDelayed(() -> start1(), 5000); // Thử lại sau 5 giây
        }
    }

}