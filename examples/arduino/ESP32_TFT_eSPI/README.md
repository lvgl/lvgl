# Example for lv_arduino using a slider

This example has the screen set to 320x480 (ILI9488), change this by altering the following lines in the main ino file:

```C
int screenWidth = 480;
int screenHeight = 320;
```

## Backlight

Change pin 32 to your preferred backlight pin using a PNP transistor (2N3906) or remove the following code and connect directly to +ve:

```C
  ledcSetup(10, 5000/*freq*/, 10 /*resolution*/);
  ledcAttachPin(32, 10);
  analogReadResolution(10);
  ledcWrite(10,768);
```

## Theme selection

Change the following to change the theme:

```C
  lv_theme_t * th = lv_theme_night_init(210, NULL);     //Set a HUE value and a Font for the Night Theme
  lv_theme_set_current(th);
```

## Calibration

This is using the bodmer tft_espi driver for touch. To correctly set the calibration load the calibration sketch and replace the following with your values:

```C
uint16_t calData[5] = { 275, 3620, 264, 3532, 1 };
```

## Screen rotation

Check the following if you need to alter your screen rotation:

```C
  tft.setRotation(3);
```
