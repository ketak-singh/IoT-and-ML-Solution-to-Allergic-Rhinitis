# So this project has 3 components.
### 1) Hardware- the wristband
### 2) Cloud- storing sensor data coming from wristband
### 3) Backend- machine learning code which predicts the allergy by taking the wristband inputs from the cloud and the result is again stored into the cloud database.

The wristband then fetches the result of machine learning from the cloud database.
If the result says that there is going to be an allergy then the buzzer present in the wristband will start to ring.
That's all about the working.

# Detailed explanation:


## Hardware:
### We are using 2 sensors: 

### AQI (dust) sensor- GP2Y1010

### Temperature and humidity sensor- DHT11 

### and one Buzzer.

All these will be connected to the nodemcu development board.


I have written the code for the nodemcu in Arduino IDE in which i am simply taking the inputs from the sensors and sending it to the cloud.

## Cloud:

We are using the Firebase cloud service which is a google service.

In this database the sensor data will be stored which will be given to the backend ML code and the output will be stored in this same database which will be accesed by the nodemcu.

## Backend:

I have written a python code for the machine learning on google collab platform.
The machine learning algorithm we are using is the RANDOM FOREST CLASSIFIER because apparently for disease prediction stuff classifier algorithms are preferable and random forest also has a good accuracy compared to other algorithms.

The inputs for this machine learning model (random forest) are AQI, ATMOSPHERIC PRESSURE, TEMPERATURE, HUMIDITY, LATITUDE, LONGITUDE, WINDSPEED.

So based on these input values the Random forest CLASSIFIER will predict allergy. The output can be '1' or '0'
'1' means allergy will happen
'0' means allergy won't happen.
