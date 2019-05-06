
import smtplib
import serial
import time
import datetime
from getpass import getpass


print("Please Note User Must Have Gmail account set to app password permission order to use program if 2FA is enabled otherwise normal password")
gmail_user = input("Enter Email:")
#note user must have an app password or have 2FA off 
gmail_app_password = getpass("Enter App Password:")
usercom = input("Please Enter COMPORT: ").upper() #seems to always be COM9 or 8 on my PC can adjust as needed
#define serial port
ser = serial.Serial(usercom, 9600)
#email send function
def sendEmail(char):
    sentFrom = gmail_user
    sentTo = gmail_user
    header = 'To:' + sentTo + '\n' + 'From: ' + gmail_user
    subject = "Plant Test"
    header = header + '\n' + 'Subject:' + subject + '\n'
    if char == 'L':
        body = "Low Light Levels Detected in the Enclosure\nGood Day,\nSmart Pot Monitoring Environment\n"
    if char == 'S':
        body  = "Low Soil Moisture Levels Detected in the Enclosure\nGood Day,\nSmart Pot Monitoring Environment\n"
    if char == 'T':
        body  = "Temperature Outside of Safe Range in Enclosure\nGood Day,\nSmart Pot Monitoring Environment\n"
    if char == 'H':
        body  = "Humidity Outside of Safe Range in Enclosure\nGood Day,\nSmart Pot Monitoring Environment\n"
    if char == 'W':
        body  = "URGET REPEATED WARNINGS ISSUED FOR ENVIRONMENT CHECK IMMEDIATELY\nGood Day,\nSmart Pot Monitoring Environment\n"

    emailMSG = header + '\n' + body + ' \n\n'

    # Details: http://www.samlogic.net/articles/smtp-commands-reference.htm

    #different try clauses for diff email providers?
    try:
        server = smtplib.SMTP_SSL('smtp.gmail.com', 465)
        server.ehlo()
        server.login(gmail_user, gmail_app_password)
        server.sendmail(sentFrom, sentTo, emailMSG)
        server.close()
        print('Email sent!')

    except Exception as exception:
        print("Could Not Send Email See Following Error Code:")
        print("Error: %s!\n\n" % exception)
warnCounter = 0 
filename = datetime.datetime.now().strftime("%m-%d-%Y-%H-%M") + '.csv'
file = open(filename, "w")
file.write("Date,Time,LightReading,SoilMoistureReading,TemperatureReading,HumidityReading")
file.write("\n")
file.close()
try:
    while True:
        x = datetime.datetime.now()
        date = x.strftime("%x")
        time = x.strftime("%X")

        file = open(filename, "a")
        ser.flushInput()
        message = ser.readline().decode('utf-8')
        print(message)
        message2 = message[0:len(message) - 6]
        print(message2)
        file.write(date)
        file.write(",")
        file.write(time)
        file.write(",")
        file.write(message2)
        file.write("\n")

        if message[-1] :
            sendEmail("S")
            warnCounter = warnCounter + 1
        elif message[-2] :
            sendEmail("L")
            warnCounter = warnCounter + 1
        elif message[-3] :
            sendEmail("T")
            warnCounter = warnCounter + 1
        elif message[-4] :
            sendEmail("H")
            warnCounter = warnCounter + 1
        elif warnCounter >= 5 :
            sendEmail("W")
            print("REPEATED WARNING SIGNALS SENT! CHECK ENVIRONMENT! MONITOR WILL NOW SHUT DOWN.")
            exit()
        else:
            print("Plant is currently in optimal environment")

        file.close() 
except KeyboardInterrupt:
    pass
    exit()

    
