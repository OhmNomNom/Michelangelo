import serial,time
ser = serial.Serial(6, baudrate=9600)

print(ser.name)

file = open("Output.txt","w")

while True:
    ser.write(b"M105\n")
    ser.flush()
    resp = ser.readline().decode();
    t = time.clock()
    resp = resp[resp.find('T')+1:].split()[0]
    print(str(t) + " " + resp)
    file.write(str(t) + " " + resp + "\n")
    file.flush()
    time.sleep(0.1)
    
input()