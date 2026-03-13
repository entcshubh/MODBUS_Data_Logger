#include "Global.h"

void handleRoot()
{
  // ✅ Always show STA IP (connected WiFi IP)
  String ipStr = (WiFi.status() == WL_CONNECTED) ? WiFi.localIP().toString() : "Not connected";
  String macStr = WiFi.macAddress();

  String page = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>NODE Setup</title>

<style>
  body { margin:0; font-family: Arial; background:#f3f4f6; }

  .topbar {
    height: 55px;
    background: #111827;
    color: white;
    display: flex;
    align-items: center;
    padding: 0 15px;
    gap: 12px;
    position: sticky;
    top: 0;
    z-index: 2000;
  }

  .topbar h2 { margin:0; font-size:18px; color:#60a5fa; }

  .menu-btn {
    font-size: 24px;
    cursor: pointer;
    padding: 8px 12px;
    border-radius: 10px;
    background: #1f2937;
    user-select: none;
  }
  .menu-btn:hover { background: #374151; }

  .layout { display:flex; height: calc(100vh - 55px); }

  .sidebar {
    width: 230px;
    background:#111827;
    color:white;
    padding:15px;
    transition: transform 0.3s ease;
    display:flex;
    flex-direction: column;
  }

  .sidebar h3 { text-align:center; color:#93c5fd; margin-top: 0; }

  .item {
    background:#1f2937;
    padding:12px;
    border-radius:10px;
    margin:10px 0;
    cursor:pointer;
    user-select:none;
  }
  .item:hover { background:#374151; }

  .spacer { flex: 1; }

  .content { flex:1; padding:20px; overflow:auto; }

  .card {
    background:white;
    padding:20px;
    max-width:580px;
    border-radius:12px;
    box-shadow:0px 0px 12px rgba(0,0,0,0.1);
    margin: auto;
  }

  input, select {
    width:100%;
    padding:10px;
    margin:8px 0;
    border-radius:10px;
    border:1px solid #ccc;
    font-size:15px;
    box-sizing: border-box;
  }

  button {
    width:100%;
    padding:12px;
    border:none;
    border-radius:10px;
    background:#22c55e;
    color:white;
    font-size:16px;
    cursor:pointer;
  }
  button:hover { background:#16a34a; }

  .hidden { display:none; }

  .overlay {
    display:none;
    position: fixed;
    top:55px;
    left:0;
    right:0;
    bottom:0;
    background: rgba(0,0,0,0.4);
    z-index: 1500;
  }

  @media (max-width: 768px) {
    .sidebar {
      position: fixed;
      top:55px;
      left:0;
      height: calc(100vh - 55px);
      z-index: 1800;
      transform: translateX(-260px);
      width: 240px;
    }
    .sidebar.open { transform: translateX(0px); }
    .overlay.show { display:block; }
    .content { padding: 15px; }
    .card { max-width: 100%; margin: 0; }
  }

  .note { font-size:14px; color:#555; line-height:1.5; }

  .toggle-wrap { display:flex; gap:10px; margin: 10px 0; }

  .toggle-btn {
    flex:1;
    padding:10px;
    border-radius:10px;
    border:1px solid #ccc;
    cursor:pointer;
    background:#f9fafb;
    text-align:center;
    font-weight:600;
  }

  .toggle-btn.active {
    background:#2563eb;
    color:white;
    border:1px solid #2563eb;
  }

  .login-box {
    background:white;
    padding:25px;
    border-radius:12px;
    max-width:380px;
    margin: 50px auto;
    box-shadow:0px 0px 12px rgba(0,0,0,0.1);
  }

  .login-title { text-align:center; color:#2563eb; margin-top:0; }

  .pass-wrap { position: relative; width: 100%; }
  .pass-wrap input { padding-right: 70px; }

  .show-btn {
    position: absolute;
    right: 8px;
    top: 50%;
    transform: translateY(-50%);
    width: 60px;
    height: 34px;
    border-radius: 10px;
    border: none;
    background: #e5e7eb;
    cursor: pointer;
    font-size: 12px;
    font-weight: bold;
    color: #111827;
    display:flex;
    align-items:center;
    justify-content:center;
    padding:0;
  }
  .show-btn:hover { background: #d1d5db; }

  .toast {
    position: fixed;
    bottom: 18px;
    left: 50%;
    transform: translateX(-50%);
    background: #111827;
    color: white;
    padding: 12px 16px;
    border-radius: 14px;
    font-size: 14px;
    box-shadow:0px 0px 18px rgba(0,0,0,0.25);
    display:none;
    z-index: 9999;
    text-align:center;
    max-width: 92%;
  }

  .info-row {
    background:#f9fafb;
    padding:12px;
    border-radius:12px;
    margin:10px 0;
    border:1px solid #e5e7eb;
  }
  .info-row b { color:#111827; }
  .info-row small { color:#6b7280; }

  .btn-row { display:flex; gap:10px; margin-top:14px; }
  .btn-row button { width: 100%; }

  .big-green { background:#16a34a; font-weight:bold; font-size:18px; padding:14px; }
  .big-green:hover { background:#15803d; }

  .btn-gray { background:#6b7280; }
  .btn-gray:hover { background:#4b5563; }
</style>
</head>

<body>

<div class="toast" id="toast">Saved</div>

<!-- LOGIN PAGE -->
<div id="loginPage" class="login-box">
  <h2 class="login-title">NODE Setup Login</h2>

  <label>Username</label>
  <input type="text" id="loginUser" placeholder="Enter username">

  <label>Password</label>
  <div class="pass-wrap">
    <input type="password" id="loginPass" placeholder="Enter password">
    <button class="show-btn" type="button" onclick="toggleShow('loginPass', this)">SHOW</button>
  </div>

  <button type="button" onclick="doLogin()">Login</button>

  <p class="note" style="margin-top:12px;">
    Default Login: <b>admin / admin</b>
  </p>
</div>

<!-- MAIN UI -->
<div id="mainUI" class="hidden">

  <div class="topbar">
    <div class="menu-btn" onclick="toggleSidebar()">&#8942;</div>
    <h2>NODE Setup</h2>
  </div>

  <div class="overlay" id="overlay" onclick="closeSidebar()"></div>

  <div class="layout">

    <div class="sidebar" id="sidebar">
      <h3>Menu</h3>

      <div class="item" onclick="openTab('general')">General Info</div>
      <div class="item" onclick="openTab('wifi')">WiFi Setup</div>
      <div class="item" onclick="openTab('mqtt')">MQTT Setup</div>
      <div class="item" onclick="openTab('http')">HTTP Setup</div>
      <div class="item" onclick="openTab('modbus')">Modbus Setup</div>

      <div class="spacer"></div>
    </div>

    <div class="content">

      <!-- GENERAL INFO -->
      <div id="general" class="card hidden">
        <h2>General Info</h2>
        <p class="note">
          Save WiFi/MQTT/HTTP/Modbus first.  
          Then check here.  
          If everything is correct, press <b>Update & Restart</b>.
        </p>

        <div class="info-row"><b>Node IP:</b><br><small id="infoIP">)rawliteral";
  page += ipStr;

  page += R"rawliteral(</small></div>

        <div class="info-row"><b>ESP32 MAC:</b><br><small id="infoMAC">)rawliteral";

  page += macStr;

  page += R"rawliteral(</small></div>

        <div class="info-row"><b>WiFi:</b><br>
          <small>
            SSID: <span id="infoSSID">)rawliteral";

  page += String(config.wifiSSID);

  page += R"rawliteral(</span><br>
            Hidden: <span id="infoHidden">)rawliteral";

  page += String(config.wifiHidden ? "YES" : "NO");

  page += R"rawliteral(</span><br>
            EncMode: <span id="infoEncMode">)rawliteral";

  page += String(config.wifiEncMode);

  page += R"rawliteral(</span><br>
            EncType: <span id="infoEncType">)rawliteral";

  page += String(config.wifiEncType);

  page += R"rawliteral(
            </span>
          </small>
        </div>

        <div class="info-row"><b>MQTT:</b><br>
          <small>
            Host: <span id="infoMqttHost">)rawliteral";

  page += String(config.mqttHost);

  page += R"rawliteral(</span><br>
            Port: <span id="infoMqttPort">)rawliteral";

  page += String(config.mqttPort);

  page += R"rawliteral(</span><br>
            User: <span id="infoMqttUser">)rawliteral";

  page += String(config.mqttUser);

  page += R"rawliteral(</span><br>
            Topic: <span id="infoMqttTopic">)rawliteral";

  page += String(config.mqttTopic);

  page += R"rawliteral(</span><br>
            ClientName: <span id="infoMqttClientName">)rawliteral";

  page += String(config.mqttClientName);

  page += R"rawliteral(</span><br>
            ClientID: <span id="infoMqttClientID">)rawliteral";

  page += String(config.mqttClientID);

  page += R"rawliteral(</span><br>
            QoS: <span id="infoMqttQoS">)rawliteral";

  page += String(config.mqttQoS);

  page += R"rawliteral(</span><br>
            KeepAlive: <span id="infoMqttKeepAlive">)rawliteral";

  page += String(config.mqttKeepAlive);

  page += R"rawliteral(
            </span>
          </small>
        </div>

        <div class="info-row"><b>HTTP:</b><br>
          <small>
            URL: <span id="infoHttpURL">)rawliteral";

  page += String(config.httpURL);

  page += R"rawliteral(</span><br>
            Port: <span id="infoHttpPort">)rawliteral";

  page += String(config.httpPort);

  page += R"rawliteral(
            </span>
          </small>
        </div>

        <div class="info-row"><b>Modbus:</b><br>
          <small>
            SlaveID: <span id="infoSlaveID">)rawliteral";

  page += String(config.slaveID);

  page += R"rawliteral(</span><br>
            Baudrate: <span id="infoBaud">)rawliteral";

  page += String(config.baudrate);

  page += R"rawliteral(</span><br>
            Parity: <span id="infoParity">)rawliteral";

  page += String(config.parity);

  // page += R"rawliteral(</span><br>
  //           RegType: <span id="infoRegType">)rawliteral";

  // page += String(config.regType);

  // page += R"rawliteral(</span><br>
  //           RegList: <span id="infoRegList">)rawliteral";

  // page += String(config.regList);

   page += R"rawliteral(
<div class="info-row"><b>Modbus:</b><br>
  <small>
    SlaveID:
    <span id="infoSlaveID">)rawliteral";

page += String(config.slaveID);

page += R"rawliteral(</span><br>

    Baudrate:
    <span id="infoBaud">)rawliteral";

page += String(config.baudrate);

page += R"rawliteral(</span><br>

    Parity:
    <span id="infoParity">)rawliteral";

page += String(config.parity);

page += R"rawliteral(</span><br><br>

    <b>Register Groups</b><br>

    <div id="infoRegisterGroups"
         style="background:#f3f4f6;
                padding:10px;
                border-radius:10px;
                margin-top:6px;">
        Waiting for configuration...
    </div>

  </small>
</div>
)rawliteral";

  page += R"rawliteral(
            </span>
          </small>
        </div>

        <div class="btn-row">
          <button class="big-green" type="button" onclick="updateAndRestart()">Update & Restart</button>
        </div>

        <div class="btn-row">
          <button class="btn-gray" type="button" onclick="openTab('wifi')">Back</button>
        </div>
      </div>

      <!-- WIFI -->
      <div id="wifi" class="card">
        <h2>WiFi Setup</h2>
        <p class="note">Press Save. Then check General Info.</p>

        <label>Network Mode</label>
        <div class="toggle-wrap">
          <div class="toggle-btn" id="btnPublic" onclick="setWiFiMode('public')">Public</div>
          <div class="toggle-btn" id="btnHidden" onclick="setWiFiMode('hidden')">Hidden</div>
        </div>

        <form id="formWiFi">
          <input type="hidden" id="wifiMode" name="wifiMode" value="public">

          <label>WiFi Encryption Mode</label>
          <select name="wifiEncMode" id="wifiEncMode" onchange="wifiEncChanged()">
)rawliteral";

  page += "<option value='NONE'" + String(String(config.wifiEncMode) == "NONE" ? " selected" : "") + ">NONE (Open Network)</option>";
  page += "<option value='WPA2_PERSONAL'" + String(String(config.wifiEncMode) == "WPA2_PERSONAL" ? " selected" : "") + ">WPA2-Personal</option>";
  page += "<option value='WPA2_WPA3_PERSONAL'" + String(String(config.wifiEncMode) == "WPA2_WPA3_PERSONAL" ? " selected" : "") + ">WPA2/WPA3-Personal</option>";
  page += "<option value='WPA3_PERSONAL'" + String(String(config.wifiEncMode) == "WPA3_PERSONAL" ? " selected" : "") + ">WPA3-Personal</option>";
  page += "<option value='WPA2_ENTERPRISE'" + String(String(config.wifiEncMode) == "WPA2_ENTERPRISE" ? " selected" : "") + ">WPA2-Enterprise (EAP)</option>";

  page += R"rawliteral(
          </select>

          <label>WiFi Encryption Type</label>
          <select name="wifiEncType">
)rawliteral";

  page += "<option value='AES'" + String(String(config.wifiEncType) == "AES" ? " selected" : "") + ">AES</option>";
  page += "<option value='TKIP_AES'" + String(String(config.wifiEncType) == "TKIP_AES" ? " selected" : "") + ">TKIP + AES</option>";
  page += "<option value='TKIP'" + String(String(config.wifiEncType) == "TKIP" ? " selected" : "") + ">TKIP</option>";

  page += R"rawliteral(
          </select>

          <label>WiFi SSID</label>
          <input type="text" name="wifiSSID" value=")rawliteral";

  page += String(config.wifiSSID);

  page += R"rawliteral(">

          <div id="wifiPassBlock">
            <label>WiFi Password</label>
            <div class="pass-wrap">
              <input type="password" id="wifiPASS" name="wifiPASS" value=")rawliteral";

  page += String(config.wifiPASS);

  page += R"rawliteral(">
              <button class="show-btn" type="button" onclick="toggleShow('wifiPASS', this)">SHOW</button>
            </div>
          </div>

          <button type="submit">Save WiFi</button>
        </form>
      </div>

      <!-- MQTT -->
      <div id="mqtt" class="card hidden">
        <h2>MQTT Setup</h2>
        <p class="note">Press Save. Then check General Info.</p>

        <form id="formMQTT">
          <label>MQTT Host</label>
          <input type="text" name="mqttHost" value=")rawliteral";

  page += String(config.mqttHost);

  page += R"rawliteral(">

          <label>MQTT Port</label>
          <input type="number" name="mqttPort" value=")rawliteral";

  page += String(config.mqttPort);

  page += R"rawliteral(">

          <label>MQTT Username</label>
          <input type="text" name="mqttUser" value=")rawliteral";

  page += String(config.mqttUser);

  page += R"rawliteral(">

          <label>MQTT Password</label>
          <div class="pass-wrap">
            <input type="password" id="mqttPass" name="mqttPass" value=")rawliteral";

  page += String(config.mqttPass);

  page += R"rawliteral(">
            <button class="show-btn" type="button" onclick="toggleShow('mqttPass', this)">SHOW</button>
          </div>

          <label>MQTT Client Name</label>
          <input type="text" name="mqttClientName" value=")rawliteral";

  page += String(config.mqttClientName);

  page += R"rawliteral(">

          <label>MQTT Client ID</label>
          <input type="text" name="mqttClientID" value=")rawliteral";

  page += String(config.mqttClientID);

  page += R"rawliteral(">

          <label>MQTT QoS</label>
          <select name="mqttQoS">
)rawliteral";

  page += "<option value='0'" + String(config.mqttQoS == 0 ? " selected" : "") + ">QoS 0</option>";
  page += "<option value='1'" + String(config.mqttQoS == 1 ? " selected" : "") + ">QoS 1</option>";
  page += "<option value='2'" + String(config.mqttQoS == 2 ? " selected" : "") + ">QoS 2</option>";

  page += R"rawliteral(
          </select>

          <label>MQTT KeepAlive (seconds)</label>
          <input type="number" name="mqttKeepAlive" value=")rawliteral";

  page += String(config.mqttKeepAlive);

  page += R"rawliteral(">

          <label>MQTT Topic</label>
          <input type="text" name="mqttTopic" value=")rawliteral";

  page += String(config.mqttTopic);

  page += R"rawliteral(">

          <button type="submit">Save MQTT</button>
        </form>
      </div>

      <!-- HTTP -->
      <div id="http" class="card hidden">
        <h2>HTTP Setup</h2>
        <p class="note">Press Save. Then check General Info.</p>

        <form id="formHTTP">
          <label>HTTP URL</label>
          <input type="text" name="httpURL" value=")rawliteral";

  page += String(config.httpURL);

  page += R"rawliteral(">

          <label>HTTP Port</label>
          <input type="number" name="httpPort" value=")rawliteral";

  page += String(config.httpPort);

  page += R"rawliteral(">

          <button type="submit">Save HTTP</button>
        </form>
      </div>

<!-- MODBUS -->
<div id="modbus" class="card hidden">
<h2>Modbus Setup</h2>

<p class="note">
Configure slave communication and registers.<br>
Press + to add register groups.
</p>

<form id="formModbus">

<h3>Device Settings</h3>

<label>Slave ID</label>
<input type="number" name="slaveID" value=")rawliteral";

page += String(config.slaveID);

page += R"rawliteral(">

<label>Baudrate</label>
<select name="baudrate">
)rawliteral";

page += "<option value='4800'"  + String(config.baudrate==4800?" selected":"")  + ">4800</option>";
page += "<option value='9600'"  + String(config.baudrate==9600?" selected":"")  + ">9600</option>";
page += "<option value='19200'" + String(config.baudrate==19200?" selected":"") + ">19200</option>";
page += "<option value='115200'"+ String(config.baudrate==115200?" selected":"")+ ">115200</option>";

page += R"rawliteral(
</select>

<label>Parity</label>
<select name="parity">
)rawliteral";

page += "<option value='NONE'" + String(config.parity=="NONE"?" selected":"") + ">NONE</option>";
page += "<option value='EVEN'" + String(config.parity=="EVEN"?" selected":"") + ">EVEN</option>";
page += "<option value='ODD'"  + String(config.parity=="ODD" ?" selected":"") + ">ODD</option>";

page += R"rawliteral(
</select>

<hr style="margin:20px 0">

<h3>Register Configuration</h3>

<div id="registerContainer"></div>

<br><br>

<button type="submit">Save Modbus</button>

</form>
</div>

<div class="info-row"><b>Runtime Status:</b><br>
  <small>
    WiFi: <span id="runWifi">-</span><br>
    MQTT: <span id="runMqtt">-</span><br>
    Modbus: <span id="runModbus">-</span><br>
    State: <span id="runState">-</span>
  </small>
</div>
</div>

<script>
var savedWiFiMode = ")rawliteral";

  page += String(config.wifiHidden ? "hidden" : "public");

  page += R"rawliteral(";

function doLogin(){
  var u = document.getElementById("loginUser").value.trim();
  var p = document.getElementById("loginPass").value.trim();

  console.log(u,p);

  if(u === "admin" && p === "admin"){
    document.getElementById("loginPage").classList.add("hidden");
    document.getElementById("mainUI").classList.remove("hidden");
    openTab("general");
  } else {
    alert("Invalid Login");
  }
}

function wifiEncChanged(){
  var enc = document.getElementById("wifiEncMode").value;
  var passBlock = document.getElementById("wifiPassBlock");

  if(enc === "NONE"){
    passBlock.style.display = "none";
    document.getElementById("wifiPASS").value = "";
  } else {
    passBlock.style.display = "block";
  }
}

function toggleSidebar(){
  var sidebar = document.getElementById("sidebar");
  var overlay = document.getElementById("overlay");
  sidebar.classList.toggle("open");
  overlay.classList.toggle("show");
}

function closeSidebar(){
  document.getElementById("sidebar").classList.remove("open");
  document.getElementById("overlay").classList.remove("show");
}

function openTab(tab){
  document.getElementById("general").classList.add("hidden");
  document.getElementById("wifi").classList.add("hidden");
  document.getElementById("mqtt").classList.add("hidden");
  document.getElementById("http").classList.add("hidden");
  document.getElementById("modbus").classList.add("hidden");

  document.getElementById(tab).classList.remove("hidden");
  closeSidebar();
}

function toggleShow(id, btn){
  var x = document.getElementById(id);
  if(x.type === "password"){ x.type = "text"; btn.innerHTML = "HIDE"; }
  else { x.type = "password"; btn.innerHTML = "SHOW"; }
}

function setWiFiMode(mode){
  document.getElementById("wifiMode").value = mode;
  document.getElementById("btnPublic").classList.remove("active");
  document.getElementById("btnHidden").classList.remove("active");
  if(mode === "public") document.getElementById("btnPublic").classList.add("active");
  else document.getElementById("btnHidden").classList.add("active");
}

function toast(msg){
  var t = document.getElementById("toast");
  t.innerHTML = msg;
  t.style.display = "block";
  setTimeout(function(){ t.style.display = "none"; }, 2600);
}

async function refreshRuntimeStatus(){
  try{
    const res = await fetch("/status");
    const s = await res.json();

    document.getElementById("runWifi").innerHTML = s.wifi ? "Connected" : "Disconnected";
    document.getElementById("runMqtt").innerHTML = s.mqtt ? "Connected" : "Disconnected";
    document.getElementById("runModbus").innerHTML = s.modbus ? "Ready" : "Not Ready";

    let stateName = "-";
    switch(s.state){
      case 0: stateName="WIFI"; break;
      case 1: stateName="NETWORK"; break;
      case 2: stateName="MODBUS"; break;
      case 3: stateName="RUNNING"; break;
      case 4: stateName="HOTSPOT"; break;
    }
    document.getElementById("runState").innerHTML = stateName;

  } catch(e){
    console.log("runtime status error", e);
  }
}

setInterval(refreshRuntimeStatus, 2000);

// async function refreshGeneralInfo(){
//   try{
//     const res = await fetch("/get_config");
//     const cfg = await res.json();

//     document.getElementById("infoSSID").innerHTML = cfg.wifiSSID;
//     document.getElementById("infoHidden").innerHTML = cfg.wifiHidden ? "YES" : "NO";
//     document.getElementById("infoEncMode").innerHTML = cfg.wifiEncMode;
//     document.getElementById("infoEncType").innerHTML = cfg.wifiEncType;

//     document.getElementById("infoMqttHost").innerHTML = cfg.mqttHost;
//     document.getElementById("infoMqttPort").innerHTML = cfg.mqttPort;
//     document.getElementById("infoMqttUser").innerHTML = cfg.mqttUser;
//     document.getElementById("infoMqttTopic").innerHTML = cfg.mqttTopic;

//     document.getElementById("infoMqttClientName").innerHTML = cfg.mqttClientName;
//     document.getElementById("infoMqttClientID").innerHTML = cfg.mqttClientID;
//     document.getElementById("infoMqttQoS").innerHTML = cfg.mqttQoS;
//     document.getElementById("infoMqttKeepAlive").innerHTML = cfg.mqttKeepAlive;

//     document.getElementById("infoHttpURL").innerHTML = cfg.httpURL;
//     document.getElementById("infoHttpPort").innerHTML = cfg.httpPort;

//     document.getElementById("infoSlaveID").innerHTML = cfg.slaveID;
//     document.getElementById("infoBaud").innerHTML = cfg.baudrate;
//     document.getElementById("infoParity").innerHTML = cfg.parity;
//     document.getElementById("infoRegType").innerHTML = cfg.regType;
//     document.getElementById("infoRegList").innerHTML = cfg.regList;

//   } catch(e){
//     console.log("refreshGeneralInfo error", e);
//   }
// }

async function refreshGeneralInfo(){
  try{
    const res = await fetch("/get_config");
    const cfg = await res.json();

    // ================= WIFI =================
    document.getElementById("infoSSID").innerHTML = cfg.wifiSSID || "-";
    document.getElementById("infoHidden").innerHTML = cfg.wifiHidden ? "YES" : "NO";
    document.getElementById("infoEncMode").innerHTML = cfg.wifiEncMode || "-";
    document.getElementById("infoEncType").innerHTML = cfg.wifiEncType || "-";

    // ================= MQTT =================
    document.getElementById("infoMqttHost").innerHTML = cfg.mqttHost || "-";
    document.getElementById("infoMqttPort").innerHTML = cfg.mqttPort || "-";
    document.getElementById("infoMqttUser").innerHTML = cfg.mqttUser || "-";
    document.getElementById("infoMqttTopic").innerHTML = cfg.mqttTopic || "-";

    document.getElementById("infoMqttClientName").innerHTML = cfg.mqttClientName || "-";
    document.getElementById("infoMqttClientID").innerHTML = cfg.mqttClientID || "-";
    document.getElementById("infoMqttQoS").innerHTML = cfg.mqttQoS ?? "-";
    document.getElementById("infoMqttKeepAlive").innerHTML = cfg.mqttKeepAlive ?? "-";

    // ================= HTTP =================
    document.getElementById("infoHttpURL").innerHTML = cfg.httpURL || "-";
    document.getElementById("infoHttpPort").innerHTML = cfg.httpPort || "-";

    // ================= MODBUS BASIC =================
    document.getElementById("infoSlaveID").innerHTML = cfg.slaveID || "-";
    document.getElementById("infoBaud").innerHTML = cfg.baudrate || "-";
    document.getElementById("infoParity").innerHTML = cfg.parity || "-";

    // ================= REGISTER GROUP DISPLAY =================
    let regHTML = "";

    if(cfg.registerGroups && cfg.registerGroups.length > 0)
    {
      cfg.registerGroups.forEach(function(group){

        if(group.enabled)
        {
          regHTML += "<div style='margin-bottom:12px;padding:8px;background:#ffffff;border-radius:8px;border:1px solid #e5e7eb;'>";

          regHTML += "<b style='color:#111827;'>" + group.type.toUpperCase() + "</b><br>";

          if(group.reg8 && group.reg8.length > 0)
            regHTML += "<small>8 Bit: " + group.reg8.join(", ") + "</small><br>";

          if(group.reg16 && group.reg16.length > 0)
            regHTML += "<small>16 Bit: " + group.reg16.join(", ") + "</small><br>";

          regHTML += "</div>";
        }
      });

      if(regHTML === "")
        regHTML = "No register groups enabled.";
    }
    else
    {
      regHTML = "No register groups configured.";
    }

    document.getElementById("infoRegisterGroups").innerHTML = regHTML;

  }
  catch(e){
    console.log("refreshGeneralInfo error:", e);
  }
}

async function postForm(url, formId){
  const form = document.getElementById(formId);
  const fd = new FormData(form);

  const res = await fetch(url, { method: "POST", body: fd });
  const txt = await res.text();
  const obj = JSON.parse(txt);
  
  if(obj.status === "OK"){
    toast("Changes Saved ✅<br>Go to General Info and verify.");
    await refreshGeneralInfo();
  } else {
    toast("Save Failed ❌");
  }
}

// =====================================================
// MODBUS REGISTER BUILDER
// =====================================================

function loadFixedRegisterGroups()
{
  const container = document.getElementById("registerContainer");

  const registerTypes = [
    { label: "Input Register", value: "input" },
    { label: "Holding Register", value: "holding" },
    { label: "Coil", value: "coil" },
    { label: "Discrete Input", value: "discrete" }
  ];

  container.innerHTML = "";

  registerTypes.forEach((type, index) => {

    const block = document.createElement("div");
    block.className = "info-row";
    block.id = "regBlock_" + index;

    block.innerHTML = `
      <div style="display:flex;justify-content:space-between;align-items:center;">
          <b>${type.label}</b>

          <label style="display:flex;align-items:center;gap:6px;">
              <input type="checkbox" name="reg_enable_${index}">
              Enable
          </label>
      </div>

      <input type="hidden" name="regType_${index}" value="${type.value}">

      <div style="margin-top:10px;">
          <label>
              <input type="checkbox" name="reg8_enable_${index}">
              8 Bit Registers (comma separated)
          </label>

          <input type="text"
                 name="reg8_list_${index}"
                 placeholder="Example: 1,2,3,10">
      </div>

      <div style="margin-top:10px;">
          <label>
              <input type="checkbox" name="reg16_enable_${index}">
              16 Bit Registers (comma separated)
          </label>

          <input type="text"
                 name="reg16_list_${index}"
                 placeholder="Example: 40001,40002">
      </div>
    `;

    container.appendChild(block);
  });
}

//add remove function register--------------------

// let registerIndex = 0;

// function addRegisterBlock()
// {
//   const container = document.getElementById("registerContainer");

//   const block = document.createElement("div");
//   block.className = "info-row";
//   block.id = "regBlock_" + registerIndex;

//   block.innerHTML = `
//     <div style="display:flex;justify-content:space-between;align-items:center;">
//         <b>Register Group ${registerIndex+1}</b>
//         <button type="button"
//                 onclick="removeRegisterBlock(${registerIndex})"
//                 style="width:auto;background:#ef4444;padding:6px 10px;border-radius:8px;">
//             remove
//         </button>
//     </div>

//     <label>Register Type</label>
//     <select name="regType_${registerIndex}">
//       <option value="input">Input Register</option>
//       <option value="holding">Holding Register</option>
//       <option value="coil">Coil</option>
//     </select>

//     <div style="margin-top:10px;">
//         <label>
//             <input type="checkbox" name="reg8_enable_${registerIndex}">
//             8 Bit Registers (comma separated)
//         </label>

//         <input type="text"
//                name="reg8_list_${registerIndex}"
//                placeholder="Example: 1,2,3,10">
//     </div>

//     <div style="margin-top:10px;">
//         <label>
//             <input type="checkbox" name="reg16_enable_${registerIndex}">
//             16 Bit Registers (comma separated)
//         </label>

//         <input type="text"
//                name="reg16_list_${registerIndex}"
//                placeholder="Example: 40001,40002">
//     </div>
//   `;

//   container.appendChild(block);
//   registerIndex++;
// }

// function removeRegisterBlock(index)
// {
//   const block = document.getElementById("regBlock_" + index);
//   if(block) block.remove();
// }

document.getElementById("formWiFi").addEventListener("submit", function(e){
  e.preventDefault();
  postForm("/save_wifi", "formWiFi");
});

document.getElementById("formMQTT").addEventListener("submit", function(e){
  e.preventDefault();
  postForm("/save_mqtt", "formMQTT");
});

document.getElementById("formHTTP").addEventListener("submit", function(e){
  e.preventDefault();
  postForm("/save_http", "formHTTP");
});

document.getElementById("formModbus").addEventListener("submit", function(e){
  e.preventDefault();
  postForm("/save_modbus", "formModbus");
});

// document.getElementById("infoRegType").innerHTML = cfg.regType;
// document.getElementById("infoRegList").innerHTML = cfg.regList;

async function updateAndRestart(){
  toast("Updating... Restarting...");
  await fetch("/apply_now", { method:"POST" });
}

// window.onload = function(){
//   setWiFiMode(savedWiFiMode);
//   refreshGeneralInfo();
//   wifiEncChanged();
//   loadFixedRegisterGroups();
//   // addRegisterBlock();
// }

window.onload = function(){
  loadFixedRegisterGroups();
  setWiFiMode(savedWiFiMode);
  refreshGeneralInfo();
  refreshRuntimeStatus();
  wifiEncChanged();
};
</script>

</body>
</html>
)rawliteral";

  server.send(200, "text/html", page);
}