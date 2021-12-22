#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Defined pins and variables
#define startButton 13
#define resetButton 11
#define UP_BUTTON 7
#define DOWN_BUTTON 6
#define UP_BUTTON2 5
#define DOWN_BUTTON2 4
const unsigned long PADDLE_RATE = 33;
const unsigned long BALL_RATE = 30;
const uint8_t PADDLE_HEIGHT = 15;

//What score the game goes up to
int MAX = 2;

//starting temp, used for menus later
int temp = 3;

int cpuScore = 0;
int playerScore = 0;

boolean gameRun = true;
boolean resetBall = false;

boolean checkGame = false;


#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to SDA, SCL pins
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void drawCourt();

//defines where ball starts off and in which direction
uint8_t ball_x = 64, ball_y = 32;
uint8_t ball_dir_x = 1, ball_dir_y = 1;
unsigned long ball_update;

//definds where paddles are set
unsigned long paddle_update;
const uint8_t CPU_X = 12;
uint8_t cpu_y = 16;

const uint8_t PLAYER_X = 115;
uint8_t player_y = 16;

//sets up OLED screen
void setup() {
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

    
    display.display();
    unsigned long start = millis();

    //initiats all buttons
    pinMode(startButton, INPUT);
    pinMode(resetButton, INPUT);
    pinMode(UP_BUTTON, INPUT);
    pinMode(DOWN_BUTTON, INPUT);
    pinMode(UP_BUTTON2, INPUT);
    pinMode(DOWN_BUTTON2, INPUT);
    digitalWrite(startButton, 1);
    digitalWrite(resetButton,1);
    digitalWrite(UP_BUTTON,1);
    digitalWrite(DOWN_BUTTON,1);
    digitalWrite(UP_BUTTON2,1);
    digitalWrite(DOWN_BUTTON2,1);

    //clears display
    display.clearDisplay();

    //calls drawCourt function
    drawCourt();

    while(millis() - start < 2000);

    display.display();

    ball_update = millis();
    paddle_update = ball_update;
}

void loop() {

    //declares states of extra buttons
    bool update = false;
    unsigned long time = millis();

    static bool up_state = false;
    static bool down_state = false;
    static bool up_state2 = false;
    static bool down_state2 = false;
    
    up_state |= (digitalRead(UP_BUTTON) == LOW);
    down_state |= (digitalRead(DOWN_BUTTON) == LOW);
    up_state2 |= (digitalRead(UP_BUTTON2) == LOW);
    down_state2 |= (digitalRead(DOWN_BUTTON2) == LOW);

  //starts game if temp is 3, therefore only called once since changed in function
  if(temp ==3){
    //calls startGame function
    startGame(); 
  }

  //checks if checkGame is true (this is done in specific functions such as startGame
  if(checkGame){

//creates layout for "PONG" menu
  display.setCursor(25,15);
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.print("PONG");
//subheading for the main menu 
  display.setCursor(25,50);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print("START?");

  //creates new temp of 5 to call other functions
      temp = 5;
      //updates display
      update = true;
      display.display();

  //checks if temp is 5, therefore menu has been gone through
    if(temp == 5){

  //infinite loop that waits for a button to be pressed
      while(digitalRead(startButton) == HIGH)  
    {
      delay(1);
    }      

      //clears screen then calls drawCourt
      display.fillScreen(BLACK);
      drawCourt();

      //game not ready
      checkGame = false;
  
    }
  }

  //checks if ball was reset (ball is reset after first player scores)
   if(resetBall){

      //checks new temp(temp = 1 done in showScore)
      if(temp == 1){
        //clears screen then calls drawCourt
      delay(2000);
      display.fillScreen(BLACK);
      drawCourt();
      //changes temp to 0 for other if statement
      temp = 0;
      
      }

      //checks if temp is 2 from end screen
      else if(temp == 2){
      delay(3000);
      temp = 0;
      display.fillScreen(BLACK);

      //infinit while loop until reset button is pressed
      while(digitalRead(resetButton) == HIGH)  
    {
      delay(1);
    }
      //clears screan and calls draw court
      display.fillScreen(BLACK);
      drawCourt();
      }
      //ball position
      ball_x = 64, ball_y = 32;

      //when game reset give ball a random direction to go to
      do
      {

        ball_dir_x = random(-1,2);
        
      }

      while(ball_dir_x == 0);

      do
      {
        
       ball_dir_y = random(-1,2);
        
      }
      while(ball_dir_y == 0);
      resetBall = false;


      
    }

    
    //chekcs if time greater then ball_update time
    if(time > ball_update) {
      //creates new ball directions
        uint8_t new_x = ball_x + ball_dir_x;
        uint8_t new_y = ball_y + ball_dir_y;

        // Check if we hit the vertical walls for cpu
        if(new_x == 0) {
          playerScore++;

          
         //checks if max score was reached and calls gameOver function
          if(playerScore == MAX){
            
            gameOver();
          }
          //calls showScore
          else{

            showScore();
          }
        }

        //checks if other vertical wall is hit
        if(new_x == 127) {
          cpuScore++;

          
          //checks if cpuScore (player 2's) score is MAX and calls Game over screen
          if(cpuScore == MAX){

            gameOver();
          }
          else{

            showScore();
          }
        }

        // Check if we hit the horizontal walls. if hit then update ball to "bounce off"
        if(new_y == 0 || new_y == 63) {
            ball_dir_y = -ball_dir_y;
            new_y += ball_dir_y + ball_dir_y;
        }

        // Check if we hit the CPU/Player2 paddle. if hit then update ball to "bounce off"
        if(new_x == CPU_X && new_y >= cpu_y && new_y <= cpu_y + PADDLE_HEIGHT) {
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

        // Check if we hit the player paddle, update ball if hit
        if(new_x == PLAYER_X
           && new_y >= player_y
           && new_y <= player_y + PADDLE_HEIGHT)
        {
            ball_dir_x = -ball_dir_x;
            new_x += ball_dir_x + ball_dir_x;
        }

      //displays balls graphics of ball by updating old position with black to blend into background and new position to white to show up
        display.drawPixel(ball_x, ball_y, BLACK);
        display.drawPixel(new_x, new_y, WHITE);
        ball_x = new_x;
        ball_y = new_y;

         //ball speed
        ball_update += BALL_RATE;

        //update set to true so screen can update
        update = true;
    }

    
    //updates for paddles on screen
    if(time > paddle_update) {
        paddle_update += PADDLE_RATE;

        // CPU/player2 paddle
        //changes old position to black to blend into background
        display.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, BLACK);
        //checks if up button pressed
        if(up_state2) {
            cpu_y -= 1;
        }
        //checks if down button pressed
        if(down_state2) {
            cpu_y += 1;
        }
        //once button is no longer pressed proceed with these new conditions
        up_state2 = down_state2 = false;
        //updates position of paddle and shows it up on screen by colouring it white
        if(cpu_y < 1) cpu_y = 1;
        if(cpu_y + PADDLE_HEIGHT > 63) cpu_y = 63 - PADDLE_HEIGHT;
        display.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, WHITE);

        // Player 1 paddle, identical to player 2 comments
        display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
        if(up_state) {
            player_y -= 1;
        }
        if(down_state) {
            player_y += 1;
        }
        up_state = down_state = false;
        if(player_y < 1) player_y = 1;
        if(player_y + PADDLE_HEIGHT > 63) player_y = 63 - PADDLE_HEIGHT;
        display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, WHITE);

      //sets update to true for display updating
        update = true;
    }

      //checks if update is set to true so screen can update
    if(update)
        display.display();
}

//creates the court graphic for the OLED screen, this makes an empty rectangle and colours it white
void drawCourt() {

    display.drawRect(0, 0, 128, 64, WHITE);
}


//function that shows the score graphic on the OLED screen
void showScore()
{
  //stop game running and clear it
  gameRun = false;
  display.fillScreen(BLACK);
  drawCourt();

  //operations for position of "Score"
  display.setCursor(15, 4);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.println("Score");

  //operations for postion of player 2 score
  display.setCursor(20,30);
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.print(String(cpuScore));

  //operations for position of player 1 score
  display.setCursor(60,30);
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.print(String(playerScore));

//delay so it can stay on the screen for a little, so its visible to the user
 delay(2000);
 unsigned long start = millis();
 delay(2000);

//change temp to 1 in order to call specific operation in the main loop
 temp = 1;


//redraws the court, updates paddles, and sets the game to run again along with ball reset
 drawCourt();
while(millis() - start < 2000);
ball_update = millis();    
paddle_update = ball_update;
gameRun = true;
resetBall = true;

}

//function for when the MAX score has been reached by one of the players
void gameOver(){

  //sets game to stop running and clears screen
  gameRun = false;
  display.fillScreen(BLACK);
  drawCourt();

  //computes if player 1 has a greater score
  if(playerScore > cpuScore)
  {
    //operations for the position of "PLAYER 1 WON" graphic
    display.setCursor(5,4);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.print("PLAYER 1 WON");
  }else
  {
    //same as above but for player 2
    display.setCursor(5,4);
    display.setTextColor(WHITE);
    display.setTextSize(1);
    display.print("PLAYER 2 WON!");
  }
  //shows the end score of the game for both players

  //operations for player 2 score
  display.setCursor(20,25);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.print(String(cpuScore));

  //operations fopr player 1 score
  display.setCursor(60,25);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.print(String(playerScore));

  //subheading for end screen asking if the players would like to play again
  display.setCursor(5,50);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print("PLAY AGAIN?");

  //delay so the player can see the graphics before closing
  delay(2000);
  unsigned long start = millis();
  delay(2000);

  //changes temp to meet specific conditions in main loop
  temp = 2;

  //sets scores back to 0 so the game can start from scratch
  cpuScore = 0;
  playerScore = 0;


 //redraws court, paadles, and ball, also sets game to start again
 drawCourt();
while(millis() - start < 2000);
ball_update = millis();    
paddle_update = ball_update;
gameRun = true;
resetBall = true;

}

//function for main menu screen
void startGame(){
  //game not running
  gameRun = false;

  //operations for PONG graphic
  display.setCursor(25,15);
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.print("PONG");

  //subheading for main menu asking the user if they would like to start
  display.setCursor(25,50);
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.print("START?");
  
  //delays a little so the user can see graphics
  delay(2000);

  //changes temp value for specific uses in main loop
  temp = 5;

 
//sets game to true
checkGame = true;

//The contents of this function are also shown in the main loop, this is why the graphics for the game never reset here as it is never seen again.

}
