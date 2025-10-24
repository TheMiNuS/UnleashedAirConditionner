#include <Arduino.h>
#include "htmlCode.h"

// --- Paste original HTML constant arrays below (unchanged) ---
const char HTML_ROOT[] PROGMEM =  R"(
    <!DOCTYPE html>
    <html>
        <head>
            <title>UnleashedAirConditioner</title>
            <meta name='viewport' content='width=device-width, initial-scale=1'>
            <meta http-equiv='refresh' content='5;url=/'>
            <link rel="stylesheet" href="styles.css">
        </head>
        <body>
            <h1>UnleashedAirConditioner</h1>
            <div class='home-button-container'>
                <a class='button' href='/module-configuration'>Module Configuration</a>
                <a class='button' href='/read-configuration'>Read Configuration</a>
                <a class='button' href='/write-configuration'>Write Configuration</a>
                <a class='button' href='/debug-information'>Debug Information</a>
                <fieldset>
                    <legend>Status</legend>
                    <div class='form-group'>
                        <p> %isConfigurationUpToDate% </p>
                        <p> %isRemoreControlEnabled% </p>
                        <p> %heartBeat% </p>
                    </div>
                </fieldset>
            </div>
            %COPYRIGHT%
        </body>
    </html>
)";

const char HTML_MODULE_CONFIGURATION[] PROGMEM =  R"(
  <!DOCTYPE html>
  <html>
    <head>
        <meta charset='UTF-8'>
        <meta name='viewport' content='width=device-width, initial-scale=1'>
        <title>ESP Configuration page</title>
        <meta http-equiv='refresh' content='120;url=/'>
        <link rel="stylesheet" href="styles.css">
    </head>
    <body>
        <h1>ESP Configuration Page</h1>
        <div id='message'>
            <p></p>
        </div>

        <form method='get' action='/wifi' class='config-form'>
          <fieldset>
              <legend>Wifi Configuration</legend>
              <div class='form-group'>
                  <label for='wifiSSID'>SSID:</label>
                  <input type='text' id='wifiSSID' name='wifiSSID' value='%wifi_ssid%'>
              </div>
              <div class='form-group'>
                  <label for='wifiPassword'>Wifi Password:</label>
                  <input type='password' id='wifiPassword' name='wifiPassword' value='%wifi_password%'>
              </div>
          </fieldset>
          <fieldset>
              <legend>Web Interface Configuration</legend>
              <div class='form-group'>
                  <label for='httpLogin'>Login:</label>
                  <input type='text' id='httpLogin' name='httpLogin' value='%http_login%'>
              </div>
              <div class='form-group'>
                  <label for='httpPassword'>Password:</label>
                  <input type='password' id='httpPassword' name='httpPassword' value='%http_password%'>
              </div>
              <div class='form-group'>
                  <label for='hostname'>Hostname:</label>
                  <input type='text' id='hostname' name='hostname' value='%hostname%'>
              </div>
              <div class="form-group">
                  <label for="otaPassword">OTA password</label>
                  <input type="password" id="otaPassword" name="otaPassword" placeholder="Leave blank to disable OTA">
              </div>
          </fieldset>
          <fieldset>
              <legend>MQTT Client Configuration</legend>
              <div class='form-group'>
                  <label for='mqttLogin'>MQTT Login:</label>
                  <input type='text' id='mqttLogin' name='mqttLogin' value='%mqtt_login%'>
              </div>
              <div class='form-group'>
                  <label for='mqttPassword'>MQTT Password:</label>
                  <input type='password' id='mqttPassword' name='mqttPassword' value='%mqtt_password%'>
              </div>
              <div class='form-group'>
                  <label for='mqttHost'>MQTT Host:</label>
                  <input type='text' id='mqttHost' name='mqttHost' value='%mqtt_host%'>
              </div>
              <div class='form-group'>
                  <label for='mqttPort'>MQTT Port:</label>
                  <input type='text' id='mqttPort' name='mqttPort' value='%mqtt_port%'>
              </div>
          </fieldset>
          <div class='config-button-container'>
              <a href='/' class='button'>Go Back</a>
              <input type='submit' class='button' value='Save'>
          </div>
        </form>

        <fieldset>
        <legend>Firmware update</legend>
            <form method='POST' action='/doUpdate' enctype='multipart/form-data' class='config-form'>
              <div class='form-group'>
                  <input id='update' type='file' name='update'>
              </div>
              <div class='config-button-container'>
                <input type='submit' class='button' value='Firmware Update'>
              </div>
            </form>
        </fieldset>

      %COPYRIGHT%

    </body>
  </html>
)";

const char HTML_READ_CONFIGURATION[] PROGMEM =  R"(
  <!DOCTYPE html>
  <html>
  <head>
      <title>Read Configuration</title>
      <meta name='viewport' content='width=device-width, initial-scale=1'>
      <link rel="stylesheet" href="styles.css">
  </head>
  <body>
      <h1>Read Configuration</h1>
      <div class='form-group'>
          <fieldset>
              <legend>Operational Details</legend>
              <div class='form-group'>
                  <p>Return Air Temperature: %returnAirTemperature%</p>
                  <p>Operational Mode: %operationalMode%</p>
                  <p>3D Air Mode: %air3DMode%</p>
                  <p>Cleaning Request: %cleaningRequest%</p>
                  <p>Lock Mode: %lockMode%</p>
                  <p>Turbo Mode: %turboMode%</p>
              </div>
          </fieldset>
          <fieldset>
              <legend>Fan & Power Details</legend>
              <div class='form-group'>
                  <p>Fan Speed: %fanSpeed%</p>
                  <p>Power Mode: %powerMode%</p>
                  <p>Current Requested Operation Mode: %currentRequestedOperationMode%</p>
                  <p>Quiet Mode: %quietMode%</p>
                  <p>Temperature Unit: %temperatureUnit%</p>
                  <p>Temperature Set Point: %temperatureSetPoint%</p>
              </div>
          </fieldset>
          <fieldset>
              <legend>Additional Details</legend>
              <div class='form-group'>                
                  <p>Sweep Mode Left and Right: %sweepModeLR%</p>
                  <p>Sweep Mode UP and Down: %sweepModeUD%</p>
                  <p>Display: %displayMode%</p>
                  <p>Ionizer: %ionizerMode%</p>
                  <p>Auxiliary Heater: %auxiliaryHeaterMode%</p>
                  <p>Temperature Display Mode: %temperatureDisplayMode%</p>
              </div>
          </fieldset>
          <fieldset>
              <legend>Energy & Timer Details</legend>
              <div class='form-group'>
                  <p>Sleep: %sleepMode%</p>
                  <p>Energy Saving: %energySavingMode%</p>
                  <p>Remaining Timer (minutes): %remainingTimerMinutes%</p>
                  <p>Current Operation Mode: %currentOperationMode%</p>
                  <p>Unit Operating Time (hours): %unitOperatingTimeHours%</p>
                  <p>Valid Configuration: %isConfigurationUpToDate%</p>
              </div>
          </fieldset>
      </div>
      <div class='config-button-container'>
          <a href='/' class='button'>Go Back</a>
      </div>                
      %COPYRIGHT%
  </body>
  </html>
)";

const char HTML_WRITE_CONFIGURATION[] PROGMEM =  R"(
  <!DOCTYPE html>
  <html>
    <head>
        <meta charset='UTF-8'>
        <meta name='viewport' content='width=device-width, initial-scale=1'>
        <link rel="stylesheet" href="styles.css">
        <title>Write Configuration</title>
    </head>
    <body>
        <h1>Write Configuration</h1>
        <form method='get' action='/push-configuration' class='config-form'>
          <fieldset>
              <legend>Mode and Setpoint</legend>
              <div class='form-group'>
                  <label for='temperatureSetPoint'> Setpoint:</label>
                  <select id='temperatureSetPoint' name='temperatureSetPoint'>
                  %SETPOINT_VALUES%
                  </select>
              </div>
              <div class='form-group'>
                  <label for='operationMode'>Operation mode:</label>
                  <select id='operationMode' name='operationMode'>
                  %OPERATIONAL_MODE%
                  </select>
              </div>
              <div class='form-group'>
                  <label for='powerMode'>ON/OFF:</label>
                  <select id='powerMode' name='powerMode'>
                    %POWER_MODE%
                  </select>
              </div>
          </fieldset>
          <fieldset>
              <legend>FAN Speed</legend>
              <div class='form-group'>
                  <label for='fanSpeed'>Fan Speed:</label>
                  <select id='fanSpeed' name='fanSpeed'>
                    %FAN_SPEED%
                  </select>
              </div>
              <div class='form-group'>
                  <label for='turboMode'>Turbo mode:</label>
                  <select id='turboMode' name='turboMode'>
                    %TURBO_MODE%
                  </select>
              </div>
              <div class='form-group'>
                  <label for='quietMode'>Quiet mode:</label>
                  <select id='quietMode' name='quietMode'>
                    %QUIET_MODE%
                  </select>
              </div>
          </fieldset>
          <fieldset>
              <legend>Timer</legend>
              <div class='form-group'>
                  <label for='timer'>Timer (minutes):</label>
                  <input type='text' id='timer' name='timer' value='%TIMER_VALUE%'>
              </div>
          </fieldset>
          <fieldset>
              <legend>FAN Tray</legend>
              <div class='form-group'>
                  <label for='sweepModeLR'>Left and Right Sweep:</label>
                  <select id='sweepModeLR' name='sweepModeLR'>
                    %SWEEPLR%
                  </select>
              </div>
              <div class='form-group'>
                  <label for='sweepModeUD'>UP and Down Sweep:</label>
                  <select id='sweepModeUD' name='sweepModeUD'>
                    %SWEEPUD%
                  </select>
              </div>
          </fieldset>
          <fieldset>
              <legend>Other parameters</legend>
              <div class='form-group'>
                  <label for='temperatureUnit'>Temperature units:</label>
                  <select id='temperatureUnit' name='temperatureUnit'>
                    %TEMPERATURE_UNIT%
                  </select>
              </div>
              <div class='form-group'>
                  <label for='displayMode'>Display:</label>
                  <select id='displayMode' name='displayMode'>
                    %DISPLAY_MODE%
                  </select>
              </div>
              <div class='form-group'>
                  <label for='ionizerMode'>Ionizer:</label>
                  <select id='ionizerMode' name='ionizerMode'>
                    %IONIZER_MODE%
                  </select>
              </div>
              <div class='form-group'>
                  <label for='auxiliaryHeaterMode'>Auxiliary heater:</label>
                  <select id='auxiliaryHeaterMode' name='auxiliaryHeaterMode'>
                    %AUXILIARY_HEATER%
                  </select>
              </div>
              <div class='form-group'>
                  <label for='temperatureDisplayMode'>Temperature display mode:</label>
                  <select id='temperatureDisplayMode' name='temperatureDisplayMode'>
                    %TEMPERATURE_DISPLAY%
                  </select>
              </div>
              <div class='form-group'>
                  <label for='energySavingMode'>Energy saving:</label>
                  <select id='energySavingMode' name='energySavingMode'>
                    %ENERGY_SAVING%
                  </select>
              </div>
              <div class='form-group'>
                  <label for='sleepMode'>Sleep mode:</label>
                  <select id='sleepMode' name='sleepMode'>
                    %SLEEP_MODE%
                  </select>
              </div>
          </fieldset>
          <div class='config-button-container'>
              <input type='submit' class='button' value='Save'>
              <a href='/' class='button'>Go Back</a>
          </div>
        </form>
        %COPYRIGHT%
    </body>
  </html>
)";

const char HTML_PUSH_CONFIGURATION_TO_AC[] PROGMEM =  R"(
  <!DOCTYPE html>
  <html>
  <head>
      <link rel="stylesheet" href="styles.css">
      <meta name='viewport' content='width=device-width, initial-scale=1'>
      <title>Push Configuration to AC</title>
  </head>
  <body>
      <div id="loading">
          <div class="spinner"></div>
          <p>Loading...</p>
      </div>

      <script>
          // Wait for 2 seconds (2000 milliseconds)
          setTimeout(function () {
              // Redirect to /write-configuration
              window.location.href = "/write-configuration";
          }, 2000);
      </script>
  </body>
  </html>  
)";

const char HTML_PUSH_CONFIGURATION_TO_MODULE[] PROGMEM =  R"(
  <!DOCTYPE html>
  <html>
  <head>
      <meta name='viewport' content='width=device-width, initial-scale=1'>
      <link rel="stylesheet" href="styles.css">
      <title>Push Configuration to Flash and Reboot</title>
  </head>
  <body>
      <div id="loading">
          <div class="spinner"></div>
          <p>Rebooting...</p>
      </div>
        <script>
            window.location.href = '/reboot';
            setTimeout(function () {
               window.location.href = '/';
           }, 1000);
      </script>
  </body>
  </html> 
)";

const char HTML_FIRMWARE_UPGRADE_ERROR[] PROGMEM =  R"(
  <!DOCTYPE html>
  <html>
  <head>
      <meta name='viewport' content='width=device-width, initial-scale=1'>
      <link rel="stylesheet" href="styles.css">
      <title>Error during Firmware Upgrade</title>
  </head>
  <body>
      <div id="loading">
          <div class="spinner"></div>
          <p>Firmware upgrade error !</p>
      </div>
        <script>
            /* window.location.href = '/reboot'; */
            setTimeout(function () {
               window.location.href = '/';
           }, 3000);
      </script>
  </body>
  </html> 
)";

const char HTML_FIRMWARE_UPGRADE_SUCCESSFULL[] PROGMEM =  R"(
  <!DOCTYPE html>
  <html>
  <head>
      <meta name='viewport' content='width=device-width, initial-scale=1'>
      <link rel="stylesheet" href="styles.css">
      <title>Error during Firmware Upgrade</title>
  </head>
  <body>
      <div id="loading">
          <div class="spinner"></div>
          <p>Successfull upgrade: Rebooting ...</p>
      </div>
        <script>
            window.location.href = '/reboot';
            setTimeout(function () {
               window.location.href = '/';
           }, 1000);
      </script>
  </body>
  </html> 
)";

const char HTML_DEBUG_INFORMATION[] PROGMEM =  R"(
 <!DOCTYPE html>
  <html>
    <head>
        <meta charset='UTF-8'>
        <meta name='viewport' content='width=device-width, initial-scale=1'>
        <meta http-equiv='refresh' content='5;url=/debug-information'>
        <title>Debug Information</title>
        <link rel="stylesheet" href="styles.css">
    </head>
    <body>
        <h1>Debug Information</h1>
        <div class='form-group'>
          <fieldset>
            <legend>RX Frames</legend>
          <div class='form-group'>
            <p>A3 Frame :  %FrameToHexStringA3%</p>
            <p>A4 Frame :  %FrameToHexStringA4%</p>
            <p>A6 Frame :  %FrameToHexStringA6%</p>
            <p>AC Frame :  %FrameToHexStringAC%</p> 
          </div>   
          </fieldset>
          <fieldset>
            <legend>TX Frames</legend>
          <div class='form-group'>
            <p>A1 Frame :  %FrameToHexStringA1%</p>
            <p>A2 Frame :  %FrameToHexStringA2%</p>
            <p>A5 Frame :  %FrameToHexStringA5%</p>
            <p>AB Frame :  %FrameToHexStringAB%</p>      
          </div>              
          </fieldset>
        </div>  
        <div class='config-button-container'>
          <a class='button' href='/'>Go Back</a>
        </div>
      %COPYRIGHT%
    </body>
  </html>
)";

const char HTML_WAIT_FOR_VALID_CONFIGURATION[] PROGMEM =  R"(
  <!DOCTYPE html>
  <html>
  <head>
      <meta http-equiv='refresh' content='6'>
      <meta name='viewport' content='width=device-width, initial-scale=1'>
      <link rel="stylesheet" href="styles.css">
      <title>Push Configuration to Flash and Reboot</title>
  </head>
  <body>
      <div class='form-group'>
          <div id="loading">
              <div class="spinner"></div>
              <p>Wait for a first configuration from AC...</p>
          </div>
          <div class='config-button-container'>
            <a class='button' href='/'>Go Back</a>
          </div>
      </div>
  </body>
  </html>
)";

const char HTML_CSS_STYLE[] PROGMEM = R"(
    body {
        background-color: #080808; /* Dark background */
        color: #D4D4D4; /* Light text color for contrast */
        font-family: Arial, sans-serif;
        text-align: center;
        display: flex;
        flex-direction: column;
        justify-content: flex-start;
        align-items: center;
        min-height: 100vh;
        margin: 0;
        padding-bottom: 60px;
    }

    /* Apply dark blue mode styles to all elements */
    *, *::before, *::after {
        box-sizing: border-box;
        background-color: #080808; /* Dark blue background for most elements */
        color: #FFFFFF; /* Light text color for most elements */
        border-color: #252526; /* Dark blue border color for most elements */
    }

    .button {
        padding: 15px 30px;
        background-color: #333; /* Slightly darker blue for buttons */
        text-decoration: none;
        font-size: 18px;
        margin: 10px;
        border-radius: 5px;
        transition: background-color 0.3s ease;
    }

    .button:hover {
        background-color: #021e70; /* Darker blue on hover for buttons */
    }

    .config-form {
        width: 80%;
        max-width: 400px;
        margin: 0 auto;
    }

    fieldset {
        border-radius: 10px;
        padding: 10px;
        margin-bottom: 20px;
        /* border-color: #2D3B53; */
    }

    legend {
        font-weight: bold;
        text-align: center;
    }

    .form-group {
        text-align: left;
    }

    .form-group label {
        display: block;
        font-weight: bold;
        margin-bottom: 5px;
        margin-top: 10px;
    }

    /* Apply a different background color for input and select elements */
    .form-group input,
    .form-group select {
        width: 100%;
        padding: 10px;
        border-radius: 5px;
        font-size: 16px;
        background-color: #1E1E1E; /* Darker background for input and select */
        color: #D4D4D4; /* Text color for input and select */
    }

    .button-container {
        text-align: center;
    }

    .config-button-container {
        text-align: center;
    }

    .home-button-container {
        text-align: center;
    }

    /* Override the display for home page buttons */
    .home-button-container .button {
        display: block;
    }

    .config-details {
        width: 80%;
        max-width: 600px;
        margin: 0 auto;
        text-align: left;
    }

    .config-group {
        border-radius: 5px;
        padding: 15px;
        margin-bottom: 20px;
      /*  border-color: #2D3B53;*/
    }

    .config-group h2 {
        font-size: 24px;
        margin-top: 0;
    }

    .config-group p {
        margin: 5px 0;
    }

    .status-dot {
        width: 20px;
        height: 20px;
        border-radius: 50%;
        display: inline-block;
        margin-right: 5px;
    }

    .green-dot {
        background-color: #28a745; /* Green */
    }

    .yellow-dot {
        background-color: #ffc107; /* Yellow */
    }

    .orange-dot {
        background-color: #fd7e14; /* Orange */
    }

    .red-dot {
        background-color: #dc3545; /* Red */
    }

    .grey-dot {
        background-color: #6c757d; /* Grey */
    }

    .status-container {
        display: flex;
        align-items: center;
        margin-bottom: 10px;
    }

    .status-text {
        margin-left: 10px;
    }

    footer {
        position: fixed;
        bottom: 0;
        width: 100%;
        background-color: #1c1c1c;
        padding: 10px 0;
    }

    footer p {
        margin: 0;
        /* color: #FFFFFF; */
        background-color: #1c1c1c;
        font-size: 14px;
    } 

    #loading {
        display: flex;
        flex-direction: column;
        align-items: center;
        justify-content: center;
        height: 66vh;
    }

    #loading p {
        margin-bottom: 10px;
    }

    .spinner {
        width: 40px;
        height: 40px;
        border-radius: 50%;  /* With 25% you have a funny square */
        border: 4px solid #333;
        border-top: 4px solid transparent;
        animation: spin 2s linear infinite;
    }

    @keyframes spin {
        0% { transform: rotate(0deg); }
        100% { transform: rotate(360deg); }
    }

)";
