#include <WiFi.h>
#include <esp_now.h>
#include <time.h>
#include <WebSocketsServer.h>
#include <ArduinoJson.h>

const char *ssid = "<YOUR_WIFI_NAME>";
const char *password = "<YOUR_WIFI_PASSWORD>";
uint8_t broadcastAddress[] = {0xCC, 0x50, 0xE3, 0xBE, 0xF2, 0x70}; // use the broadcast address of your arduino

typedef struct struct_message {
  char a[32];
  int temperatura;
  int umidadeSolo;
  int umidadeIrrigacao;
  char datetime[20];
} struct_message;

typedef struct humidity_message {
  int humidityIrrigation;
} humidity_message;

struct_message myData;
humidity_message myHumidityIrrigation;

esp_now_peer_info_t peerInfo;
WiFiServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

String createJsonData() {

  String json = "{";
  json += "\"temperatura\":" + String(myData.temperatura) + ",";
  json += "\"umidadeSolo\":" + String(myData.umidadeSolo) + ",";
  json += "\"umidadeIrrigacao\":" + String(myData.umidadeIrrigacao) + ",";
  json += "\"datetime\":\"" + (myData.datetime ? String(myData.datetime) : "--/--/---- --:--:--") + "\"";
  json += "}";
  return json;
}

void OnDataSent(const uint8_t *broadcastAddress, esp_now_send_status_t status) {
  // Serial.print("Status de envio: ");
  // Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Sucesso" : "Falha");
}

void OnDataRecv(const esp_now_recv_info *recv_info, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  String json = createJsonData();
  webSocket.broadcastTXT(json);
}

void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_CONNECTED:
      {
        String json = createJsonData();
        webSocket.sendTXT(num, json);
      }
      break;

    case WStype_TEXT:
      {
      
        String message = String((char*)payload);
        
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, message);
        
        if (!error) {
          if (doc.containsKey("humidityIrrigation")) {
            int newHumidity = doc["humidityIrrigation"];
            
            myHumidityIrrigation.humidityIrrigation = newHumidity;
            
            esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &myHumidityIrrigation, sizeof(myHumidityIrrigation));
          }
        }
      }
  }
}



String getDarkModeIcon(bool isDarkMode) {
  if (isDarkMode) {
    return R"(
<svg width="28px" height="28px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" stroke="#f2c12e">

<g id="SVGRepo_bgCarrier" stroke-width="0"/>

<g id="SVGRepo_tracerCarrier" stroke-linecap="round" stroke-linejoin="round"/>

<g id="SVGRepo_iconCarrier"> <path d="M12 17.75C10.8628 17.75 9.75106 17.4128 8.80547 16.781C7.85989 16.1491 7.1229 15.2511 6.6877 14.2004C6.25249 13.1498 6.13862 11.9936 6.36049 10.8782C6.58235 9.76284 7.12999 8.73829 7.93414 7.93414C8.73829 7.12999 9.76284 6.58235 10.8782 6.36049C11.9936 6.13862 13.1498 6.25249 14.2004 6.6877C15.2511 7.1229
 16.1491 7.85989 16.781 8.80547C17.4128 9.75106 17.75 10.8628 17.75 12C17.7474 13.5242 17.1407 14.9852 16.0629 16.0629C14.9852 17.1407 13.5242 17.7474 12 17.75ZM12 7.75C11.1594 7.75 10.3377 7.99926 9.63883 8.46626C8.93992 8.93325 8.39519 9.59701 8.07351 10.3736C7.75184 11.1502 7.66768 12.0047 7.83167 12.8291C7.99565 13.6536 8.40043 14.4108
  8.9948 15.0052C9.58917 15.5996 10.3464 16.0044 11.1709 16.1683C11.9953 16.3323 12.8498 16.2482 13.6264 15.9265C14.403 15.6048 15.0668 15.0601 15.5337 14.3612C16.0007 13.6623 16.25 12.8406 16.25 12C16.2474 10.8736 15.7987 9.79417 15.0023 8.99772C14.2058 8.20126 13.1264 7.75264 12 7.75Z" fill="#f2c12e"/> <path d="M12 5C11.8019 4.99741 11.6126 
  4.91756 11.4725 4.77747C11.3324 4.63737 11.2526 4.44811 11.25 4.25V2.75C11.25 2.55109 11.329 2.36032 11.4697 2.21967C11.6103 2.07902 11.8011 2 12 2C12.1989 2 12.3897 2.07902 12.5303 2.21967C12.671 2.36032 12.75 2.55109 12.75 2.75V4.25C12.7474 4.44811 12.6676 4.63737 12.5275 4.77747C12.3874 4.91756 12.1981 4.99741 12 5Z" fill="#f2c12e"/> 
  <path d="M12 22C11.8019 21.9974 11.6126 21.9176 11.4725 21.7775C11.3324 21.6374 11.2526 21.4481 11.25 21.25V19.75C11.25 19.5511 11.329 19.3603 11.4697 19.2197C11.6103 19.079 11.8011 19 12 19C12.1989 19 12.3897 19.079 12.5303 19.2197C12.671 19.3603 12.75 19.5511 12.75 19.75V21.25C12.7474 21.4481 12.6676 21.6374 12.5275 21.7775C12.3874 
  21.9176 12.1981 21.9974 12 22Z" fill="#f2c12e"/> <path d="M21.25 12.75H19.75C19.5511 12.75 19.3603 12.671 19.2197 12.5303C19.079 12.3897 19 12.1989 19 12C19 11.8011 19.079 11.6103 19.2197 11.4697C19.3603 11.329 19.5511 11.25 19.75 11.25H21.25C21.4489 11.25 21.6397 11.329 21.7803 11.4697C21.921 11.6103 22 11.8011 22 12C22 12.1989 21.921 
  12.3897 21.7803 12.5303C21.6397 12.671 21.4489 12.75 21.25 12.75Z" fill="#f2c12e"/> <path d="M4.25 12.75H2.75C2.55109 12.75 2.36032 12.671 2.21967 12.5303C2.07902 12.3897 2 12.1989 2 12C2 11.8011 2.07902 11.6103 2.21967 11.4697C2.36032 11.329 2.55109 11.25 2.75 11.25H4.25C4.44891 11.25 4.63968 11.329 4.78033 11.4697C4.92098 11.6103 5 11.8011 
  5 12C5 12.1989 4.92098 12.3897 4.78033 12.5303C4.63968 12.671 4.44891 12.75 4.25 12.75Z" fill="#f2c12e"/> <path d="M6.50001 7.24995C6.30707 7.2352 6.12758 7.14545 6.00001 6.99995L4.91001 5.99995C4.83844 5.92838 4.78167 5.84341 4.74293 5.7499C4.7042 5.65639 4.68427 5.55617 4.68427 5.45495C4.68427 5.35373 4.7042 5.25351 4.74293 5.16C4.78167 
  5.06649 4.83844 4.98152 4.91001 4.90995C4.98158 4.83838 5.06655 4.78161 5.16006 4.74287C5.25357 4.70414 5.3538 4.6842 5.45501 4.6842C5.55623 4.6842 5.65645 4.70414 5.74996 4.74287C5.84347 4.78161 5.92844 4.83838 6.00001 4.90995L7.00001 5.99995C7.123 6.13746 7.19099 6.31547 7.19099 6.49995C7.19099 6.68443 7.123 6.86244 7.00001 6.99995C6.87244 
  7.14545 6.69295 7.2352 6.50001 7.24995Z" fill="#f2c12e"/> <path d="M18.56 19.31C18.4615 19.3104 18.3638 19.2912 18.2728 19.2534C18.1818 19.2157 18.0993 19.1601 18.03 19.09L17 18C16.9332 17.86 16.9114 17.7028 16.9376 17.5499C16.9638 17.3971 17.0368 17.2561 17.1465 17.1464C17.2561 17.0368 17.3971 16.9638 17.55 16.9376C17.7028 16.9113 17.8601 
  16.9331 18 17L19.09 18C19.2305 18.1406 19.3094 18.3312 19.3094 18.53C19.3094 18.7287 19.2305 18.9194 19.09 19.06C19.0233 19.1355 18.9419 19.1967 18.8508 19.2397C18.7597 19.2827 18.6607 19.3066 18.56 19.31Z" fill="#f2c12e"/> <path d="M17.5 7.24995C17.3071 7.2352 17.1276 7.14545 17 6.99995C16.877 6.86244 16.809 6.68443 16.809 6.49995C16.809 
  6.31547 16.877 6.13746 17 5.99995L18 4.90995C18.1445 4.76541 18.3406 4.6842 18.545 4.6842C18.7494 4.6842 18.9455 4.76541 19.09 4.90995C19.2345 5.05449 19.3158 5.25054 19.3158 5.45495C19.3158 5.65936 19.2345 5.85541 19.09 5.99995L18 6.99995C17.8724 7.14545 17.6929 7.2352 17.5 7.24995Z" fill="#f2c12e"/> <path d="M5.44001 19.31C5.34147 19.3104 
  5.24383 19.2912 5.15282 19.2534C5.06181 19.2157 4.97926 19.1601 4.91001 19.09C4.76956 18.9494 4.69067 18.7587 4.69067 18.56C4.69067 18.3612 4.76956 18.1706 4.91001 18.03L6.00001 17C6.13997 16.9331 6.2972 16.9113 6.45006 16.9376C6.60293 16.9638 6.7439 17.0368 6.85357 17.1464C6.96324 17.2561 7.03621 17.3971 7.06244 17.5499C7.08866 17.7028 7.06685 17.86 7.00001 18L6.00001 19.09C5.92728 19.1638 5.83985 19.2216 5.74338 19.2595C5.64691 19.2974 5.54356 19.3146 5.44001 19.31Z" fill="#f2c12e"/> </g>

</svg>
)";
  } else {
    return R"(
      <svg width="24px" height="24px" viewBox="0 0 24 24" fill="none" xmlns="http://www.w3.org/2000/svg" stroke="#78caf9">

        <g id="SVGRepo_bgCarrier" stroke-width="0"/>

        <g id="SVGRepo_tracerCarrier" stroke-linecap="round" stroke-linejoin="round"/>

        <g id="SVGRepo_iconCarrier"> <path fill-rule="evenodd" clip-rule="evenodd" d="M3.39703 11.6315C3.39703 16.602 7.42647 20.6315 12.397 20.6315C15.6858 20.6315 18.5656 18.8664 20.1358 16.23C16.7285 17.3289 12.6922 16.7548 9.98282 14.0455C7.25201 11.3146 6.72603 7.28415 7.86703 3.89293C5.20697 5.47927 3.39703 8.38932 3.39703 11.6315ZM21.187
        13.5851C22.0125 13.1021 23.255 13.6488 23 14.5706C21.7144 19.2187 17.4543 22.6315 12.397 22.6315C6.3219 22.6315 1.39703 17.7066 1.39703 11.6315C1.39703 6.58874 4.93533 2.25845 9.61528 0.999986C10.5393 0.751502 11.0645 1.99378 10.5641 2.80935C8.70026 5.84656 8.83194 10.0661 11.397 12.6312C13.9319 15.1662 18.1365 15.3702 21.187 13.5851Z" fill="#78caf9"/> </g>

      </svg>
    )";
  }
}

void handleClient(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();
  client.println("<!DOCTYPE html>");
  client.println("<html lang='pt-BR'>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=1.0'>");
  client.println("<title>Horta Inteligente</title>");
  client.println("<style>");

  client.println("div.btnContainer { display: flex; flex-direction: column; gap: 15px; width: 100%; max-width: 600px; margin: 20px auto; }");

// Estilos para a grid que coloca o input à esquerda e o botão à direita
client.println(".input-button-container { display: grid; grid-template-columns: 1fr 150px; gap: 10px; align-items: center; }");

// Estilo do input de número
client.println("input[type='number'] { padding: 10px; font-size: 1.1em; border: 1px solid #ccc; border-radius: 5px; width: 100%; box-sizing: border-box; transition: border-color 0.3s; }");
client.println("input[type='number']:focus { border-color: #4CAF50; outline: none; }");

// Estilo do botão
client.println("button { padding: 12px 20px; background-color: #4CAF50; color: white; font-size: 1.1em; border: none; border-radius: 5px; cursor: pointer; transition: background-color 0.3s, transform 0.2s; }");
client.println("button:hover { background-color: #45a049; }");
client.println("button:active { transform: scale(0.98); }");

// Modo Dark e Light para os botões e inputs
client.println("body.dark input[type='number'] { background-color: #333; color: #fff; border-color: #555; }");
client.println("body.dark input[type='number']:focus { border-color: #90caf9; }");
client.println("body.dark button { background-color: #333; color: #fff; }");
client.println("body.dark button:hover { background-color: #4CAF50; }");
client.println("body.dark button:active { transform: scale(0.98); }");
  
  // Estilos gerais para Light e Dark Mode
  client.println("body { font-family: Arial, sans-serif; margin: 0; padding: 20px; display: flex; justify-content: center; align-items: center; flex-direction: column; transition: background-color 0.3s, color 0.3s; }");
  client.println("body.light { background-color: #f2f2f2; color: #333; }");
  client.println("body.dark { background-color: #121212; color: #ffffff; }");

  client.println(".title { color: #4CAF50; text-align: center; margin-bottom: 20px; }");
  client.println(".title { color: #4CAF50; text-align: center; margin-bottom: 20px; }");
  client.println(".title { font-size: 2.5em; font-weight: bold; color: #4CAF50; margin-bottom: 10px; display: flex; justify-content: center; width: 100%; }");

  // Botão de Dark Mode
  client.println(".dark-mode-toggle { position: fixed; top: 20px; right: 20px; width: 40px; height: 40px; border: none; border-radius: 50%; background-color: #e8f5e9; color: white; cursor: pointer; display: flex; justify-content: center; align-items: center; padding: 0; }");

  // Estilo do Grid para as boxes de informações
  client.println(".grid { display: grid; gap: 15px; grid-template-columns: repeat(auto-fit, minmax(300px, 1fr)); width: 100%; max-width: 1000px; padding: 10px; }");

  // Estilos das boxes informativas
  client.println(".info-box { background-color: #e8f5e9; padding: 20px; border-radius: 8px; text-align: center; box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1); }");
  client.println(".info-box h2 { font-size: 1.3em; color: #388e3c; margin-bottom: 10px; }");
  client.println(".info-box p { margin: 0; font-size: 1.1em; color: #666; }");

  client.println("#dark-mode-icon { padding-top: 4px; }");

  // Ajustes para Dark Mode nas boxes
  client.println("body.dark .info-box { background-color: #333; color: #fff; }");
  client.println("body.dark .info-box h2 { color: #73a7ce; }");

  // Media Query para telas pequenas
  client.println("@media (max-width: 600px) { .grid { grid-template-columns: 1fr; } }");
  client.println("@media (max-width: 600px) { .title { justify-content: flex-start; font-size: 1.8em; } }");

  client.println("</style>");

  // Script JavaScript para alternar entre Dark Mode e Light Mode
  client.println("<script>");
  client.println("function toggleDarkMode() {");
  client.println("  document.body.classList.toggle('dark');");
  client.println("  document.body.classList.toggle('light');");
  client.println("  const icon = document.getElementById('dark-mode-icon');");
  client.println("  const btnElement = document.getElementById('darkButton');");
  client.println("  if (btnElement.style.backgroundColor == 'rgb(51, 51, 51)') { btnElement.style.backgroundColor = '#e8f5e9' } else {btnElement.style.backgroundColor = '#333'};");
  client.println("  const titles = document.querySelectorAll('.title');");
  client.println("  if (getComputedStyle(titles[0]).color == 'rgb(76, 175, 80)') {for (let title of titles) {title.style.color='#90caf9'}} else {for (let title of titles) {title.style.color='#4CAF50'}}");
  client.println("  icon.innerHTML = document.body.classList.contains('dark') ? `" + getDarkModeIcon(true) + "` : `" + getDarkModeIcon(false) + "`;");
  client.println("}");  

  
  client.println("let ws;");
  client.println("function connectWebSocket() {");
  client.println("  ws = new WebSocket('ws://' + window.location.hostname + ':81');");
  
  client.println("  ws.onopen = function() {");
  client.println("    console.log('Conectado ao WebSocket');");
  client.println("  };");
  
  client.println("  ws.onclose = function() {");
  client.println("    console.log('Desconectado do WebSocket');");
  client.println("    setTimeout(connectWebSocket, 2000);");  // Reconecta após 2 segundos
  client.println("  };");
  
  client.println("  ws.onmessage = function(evt) {");
  client.println("    const data = JSON.parse(evt.data);");
  client.println("    updateValues(data);");
  client.println("  };");
  client.println("}");

  
  // Função para atualizar os valores na página
  client.println("function updateValues(data) {");
  client.println("  document.getElementById('temperatura').textContent = data.temperatura + '°';");
  client.println("  document.getElementById('umidadeSolo').textContent = data.umidadeSolo + '%';");
  client.println("  document.getElementById('statusSolo').textContent = data.umidadeSolo < data.umidadeIrrigacao ? 'SECO' : 'ÚMIDO';");
  client.println("  document.getElementById('umidadeIrrigacao').textContent = data.umidadeIrrigacao + '%';");
  client.println("  document.getElementById('ultimaIrrigacao').textContent = data.datetime ? data.datetime : '--/--/---- --:--:--';");
  client.println("}");
  
  // Inicia a conexão WebSocket quando a página carregar
  client.println("connectWebSocket()");
  
  // ... (mantenha o script do dark mode existente) ...
  client.println("</script>");

  client.println("<script>");
// ... (previous WebSocket connection code remains the same) ...

client.println("function updateHumidity() {");
client.println("const newHumidity = parseInt(document.getElementById('minHumidity').value);");
client.println("const message = JSON.stringify({ humidityIrrigation: newHumidity });");
client.println("ws.send(message);");
client.println("}");

client.println("</script>");

  client.println("</head>");
  client.println("<body class='light'>");  // Define o modo claro como padrão
  
  // Botão de Dark Mode
  client.println("<button id='darkButton' class='dark-mode-toggle' onclick='toggleDarkMode()'>");
  client.println("<div id='dark-mode-icon'>" + getDarkModeIcon(false) + "</div>");
  client.println("</button>");
  
  client.println("<h1 class='title'>Horta Inteligente</h1>");
  
  client.println("<div class='grid'>");

  client.println("<div class='info-box'>");
  client.println("<h2>Temperatura</h2>");
  client.println("<p id='temperatura'>--°</p>");
  client.println("</div>");

  client.println("<div class='info-box'>");
  client.println("<h2>Status do solo</h2>");
  client.println("<p id='statusSolo'>--</p>");
  client.println("</div>");

  client.println("<div class='info-box'>");
  client.println("<h2>Umidade do Solo</h2>");
  client.println("<p id='umidadeSolo'>--%</p>");
  client.println("</div>");

  client.println("<div class='info-box'>");
  client.println("<h2>Umidade para irrigação</h2>");
  client.println("<p id='umidadeIrrigacao'>--%</p>");
  client.println("</div>");

  client.println("<div class='info-box'>");
  client.println("<h2>Ultima irrigação</h2>");
  client.println("<p id='ultimaIrrigacao'>--/--/---- --:--</p>");
  client.println("</div>");

  client.println("</div>");  // Fecha a grid

 client.println("<h2 class='title'>Controle de Umidade</h2>");
client.println("<div class='btnContainer'");

client.println("<div class='input-button-container'>");
client.println("<input type='number' id='minHumidity' name='minHumidity' placeholder='Umidade mínima (%)'>");
client.println("<button type='submit' onclick='updateHumidity()'>Enviar</button>");
client.println("</div>");

client.println("</div>");


  client.println("</body>");
  client.println("</html>");
  client.println();
}



void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
  
  webSocket.begin();
  webSocket.onEvent(webSocketEvent);


  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(OnDataRecv);
  esp_now_register_send_cb(OnDataSent);

  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  if (esp_now_add_peer(&peerInfo) != ESP_OK){
    Serial.println("Failed to add peer");
    return;
  }
}

void loop() {
  webSocket.loop();

  WiFiClient client = server.available();
  if (client) {
    Serial.println("Novo cliente conectado");
    handleClient(client);

    delay(2000);
    client.stop();

    Serial.println("Cliente desconectado");
  }

  // if (result == ESP_OK) {
  //   Serial.println("Sent with success");
  // }
  // else {
  //   Serial.println("Error sending the data");
  // }

  delay(1000);
}