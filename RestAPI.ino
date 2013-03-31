/**
 * Wohnung API: REST API to collect and send data from my home.
 * 
 * Author: Nicolas Ritouet <nicolas@ritouet.com>
 *
 * Rest API code based on Jotschi's implementation (www.jotschi.de/?p=769)
**/


#include <SPI.h>
#include <Ethernet.h>
#include <RCSwitch.h>

RCSwitch mySwitch = RCSwitch();

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x00, 0x00 };
IPAddress ip(192, 168, 2, 177);

char cmd[15];
char param1[15];

float temperature = 0.0;

#define STRING_BUFFER_SIZE 128
char buffer[STRING_BUFFER_SIZE];

EthernetServer server(80);

void setup() {
  Serial.begin(9600);
  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
  mySwitch.enableReceive(0);  // Receiver on inerrupt 0 => that is pin #2
}

void send404(EthernetClient client) {
     client.println("HTTP/1.1 404 OK");
     client.println("Content-Type: text/html");
     client.println("Connnection: close");
     client.println();
     client.println("<!DOCTYPE HTML>");
     client.println("<html><body>404</body></html>");
}

void sendResponse(EthernetClient client, String response) {
 
   // Send a standard http response header
   client.println("HTTP/1.1 200 OK");
   client.println("Content-Type: application/json");
   client.println("Connnection: close");
   client.println();
   client.println("{");
   client.println(response);
   client.println("\n}");
}

/**
 * Parse the string and return an array which contains all path segments
 */
char** parse(char* str) {

  char ** messages;
	messages = (char**)malloc(sizeof(char *)); // dynamic array
	char *p;
	p = strtok(str, " ");
	unsigned int i = 0;
	while (p != NULL) {
	  p = strtok(NULL, "/");
	  char *sp;
	  boolean last = false;
	  sp = strchr(p, ' ');
	  if (sp != NULL) {
            *sp++ = '\0';
	    last = true;
	  }
	  messages[i] = p;
	  i++;
	  if (last) {
	    break;
	  }
	  messages = (char**)realloc(messages, sizeof(char *) * i + 1);
	}

	messages[i] = '\0';
	return messages;
}

void handleCommand(EthernetClient client, char* cmd, char* param) {
    if (strcmp(cmd, "temp") == 0) {
      char tempChar[10];
      dtostrf(temperature, 1, 2, tempChar);
      String strReponse = String("temp: ") + tempChar;
      sendResponse(client, strReponse);
      Serial.println(strReponse);
    } else {
      send404(client);
    }  
}

int countSegments(char* str) {
  int p = 0;
  int count = 0;
  while (str[p] != '\0') {
    if (str[p] == '/') {
      count++;
    }
    p++;
  }
  // We don't want to count the / in 'HTTP/1.1'
  count--;
  return count;
}

void loop() {
  if (mySwitch.available()) {
    int temperatureValue = mySwitch.getReceivedValue();
    temperature = (float)temperatureValue;
    mySwitch.resetAvailable();
  }
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        
        char c;
        int bufindex = 0; // reset buffer
        buffer[0] = client.read();
        buffer[1] = client.read();
        bufindex = 2;
        // Read the first line to determin the request page
        while (buffer[bufindex-2] != '\r' && buffer[bufindex-1] != '\n') { 
            // read full row and save it in buffer
            c = client.read();
            if (bufindex<STRING_BUFFER_SIZE) {
              buffer[bufindex] = c;
            }
            bufindex++;
        }
        // Clean buffer for next row
        bufindex = 0;
 
        // Parse the query string
        int nSegments = countSegments(buffer);
        char **pathsegments = parse(buffer);
 
        int i = 0;
        for(i=0; i<nSegments; i++) {
          Serial.println(pathsegments[i]);
        }

        if (c == '\n' && currentLineIsBlank) {
          handleCommand(client, pathsegments[0], pathsegments[1]);
          break;
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    // Give the web browser time to receive the data
    delay(1);
    // Close the connection:
    client.stop();
    Serial.println("Client disconnected");
  }
}
