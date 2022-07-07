
//------------------
void Stummilok_Ani()
//------------------
{
  uint8_t y = 30;
  uint8_t AnzZeichen = 20; // Wenn die Anzahl der Zeichen verändert wird, dann muss auch Make_Font_h_File.bat angepasst werden
  uint8_t j = 0;
  Display_setTextSize(11); // Stummilok
  for (int16_t i = 127; i > -50; i--, j+=2)
    {
    Display_clearDisplay();
    display.drawGlyph(i,    y-14, 'A'+ (j % AnzZeichen));
    display.drawGlyph(i+20, y-14, 'B'+ (j % AnzZeichen));
    display.drawHLine(0, y, 128);
    Display_display();
    delay(60);
    }
  Display_setTextSize(1);
}

//-------------------
void Stummilok_Ani2()
//-------------------
{
  uint8_t y = 30;
  uint8_t AnzZeichen = 20; // Wenn die Anzahl der Zeichen verändert wird, dann muss auch Make_Font_h_File.bat angepasst werden
  uint8_t j = 0;
  Display_setTextSize(14); // Stummilok2
  for (int16_t i = 127; i > -100; i--, j+=5)
    {
    Display_clearDisplay();
    if (j >= AnzZeichen) j = 0;
    for (uint8_t k = 0; k < 5; k++)
        {
        display.drawGlyph(i+k*20, y-14, 73 + k + j);
        }
    display.drawHLine(0, y+7, 128);
    display.drawHLine(0, y+8, 128);
    Display_display();
    delay(60);
    }
  Display_setTextSize(1);
}

//-------------
void Haus_Ani()
//-------------
{
  srand(millis());
  uint8_t x = rand() % (128-15);
  uint8_t y = rand() % (64-35);
  uint8_t AnzZeichen = 28; // Wenn die Anzahl der Zeichen verändert wird, dann muss auch Make_Font_h_File.bat angepasst werden
  Display_setTextSize(12); // MLL Haus
  for (int16_t i = 'A'; i < 'A'+AnzZeichen; i++)
    {
    Display_clearDisplay();
    display.drawGlyph(x, y, i);
    Display_display();
    if (i%2)
         delay(100);
    else delay(400);
    }
  Display_setTextSize(1);
}

//-------------------
void Stummilok_Ani3()
//-------------------
{
  uint8_t y = 30;
  uint8_t AnzZeichen = 30; // Wenn die Anzahl der Zeichen verändert wird, dann muss auch Make_Font_h_File.bat angepasst werden
  uint8_t j = 0;
  Display_setTextSize(15); // Stummilok3
  for (int16_t x0 = 127; x0 > -100; x0--, j+=5) 
    {
    Display_clearDisplay();
    if (j >= AnzZeichen) j = 0;
//j = 15;
    for (uint8_t k = 0; k < 5; k++) // k = 0..4
        {               //   x     y    Buchstabe          
        display.drawGlyph(x0+k*20, y-14, 73 + k + j); // Buchstaben der Lok ausgeben, j = Erster Lok Buchstabe
        }
    int16_t x0_MirZ21 = x0+5*20;
    if (x0_MirZ21 < 54) x0_MirZ21 = 54;
    display.drawGlyph(x0_MirZ21, y-14, 103); // MirZ21 Text

    display.drawHLine(0, y+9, 128);
    display.drawHLine(0, y+10, 128);
    Display_display();
    delay(60);
    }
  Display_setTextSize(1);
}

//-------------
void Haus_Ani1()
//-------------
{
  srand(millis());
  uint8_t x = rand() % (128-15);
  uint8_t y = rand() % (64-35);
  uint8_t AnzZeichen = 60; // Wenn die Anzahl der Zeichen verändert wird, dann muss auch Make_Font_h_File.bat angepasst werden
  Display_setTextSize(16); // MLL Haus
  for (int16_t i = 'A'; i < 'A'+AnzZeichen; i++)
    {
    Display_clearDisplay();
    display.drawGlyph(x, y, i);
    Display_display();
    if (i%2)
         delay(100);
    else delay(400);
    }
  Display_setTextSize(1);
}

//-------------
void Haus_Ani2()
//-------------
{
  srand(millis());
  uint8_t x = rand() % (128-15);
  uint8_t y = rand() % (64-35);
  uint8_t AnzZeichen = 58; // Wenn die Anzahl der Zeichen verändert wird, dann muss auch Make_Font_h_File.bat angepasst werden
  Display_setTextSize(17); // MLL Haus
  for (int16_t i = 'A'; i < 'A'+AnzZeichen; i++)
    {
    Display_clearDisplay();
    display.drawGlyph(x, y, i);
    Display_display();
    if (i%2)
         delay(100);
    else delay(400);
    }
  Display_setTextSize(1);
}