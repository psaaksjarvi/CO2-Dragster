import csv
import random
import sys
from PySide6 import QtCore, QtGui, QtWidgets
from PySide6.QtCore import QIODevice
from PySide6.QtGui import QPixmap
from PySide6.QtWidgets import QListWidgetItem, QComboBox, QGraphicsView
from PySide6.QtUiTools import QUiLoader
from co2_ui import Ui_MainWindow
import serial
import serial.tools.list_ports

class MyWindow(QtWidgets.QMainWindow, Ui_MainWindow):
    def __init__(self):
        self.ui = Ui_MainWindow()
        print("C02 Dragsteroff")
        QtWidgets.QMainWindow.__init__(self)
        Ui_MainWindow.__init__(self)
        self.setupUi(self)
        self.loadRacerButton.clicked.connect(self.load_home_group)
        self.addRacerButton.clicked.connect(self.add_racer)
        self.ok_racers.clicked.connect(self.racers_ok)
        self.cancel_racers.clicked.connect(self.clear_racers)
        self.race_button.clicked.connect(self.racing)
        self.delete_race_button.clicked.connect(self.delete_race)
        self.randomSelectButton.clicked.connect(self.randomly_select_racers)
        self.randomAllButton.clicked.connect(self.randomly_select_all_racers)
        self.raceNowButton.clicked.connect(self.set_race)
        self.lcdNumber.hide()
        self.lcdNumber_2.hide()
        self.arduinoLabel.hide()
        self.raceNowButton.hide()
        self.readyButton.hide()
        self.readyButton.clicked.connect(self.send_ready)
        self.arduino_ports = []
        self.ser = ""
        self.text = ""
        self.data = ""
        self.racer_1 = ""
        self.racer_2 = ""
        self.race_started = False
        self.resetRaceButton.clicked.connect(self.reset_race)
        self.resetRaceButton.hide()  # Initially hide the button
        self.populate_com_ports()
        
    def populate_com_ports(self):
        self.comboBox_2.clear()
        self.arduino_ports = [p.device for p in serial.tools.list_ports.comports()
            if 'COM' in p.description]
        self.comboBox_2.addItems(self.arduino_ports)
    
    def reset_race(self):
        self.race_started = False
        self.racer1_name.setText("")
        self.racer2_name.setText("")
        self.lcdNumber.display(0)
        self.lcdNumber_2.display(0)
        self.arduinoLabel.setText("Race Reset")
        self.readyButton.show()  # Show the "Ready" button again
        self.resetRaceButton.hide()  # Hide the "Reset Race" button
    
    def send_ready(self):
        self.readyButton.hide()
        self.ser.write('r'.encode('utf-8'))
        # wait for arduino to send Serial.println("go1");
        self.read_serial_data()
        self.race_started = True
        self.resetRaceButton.show()
    
    def set_race(self):
        #set names up top again
        racing = self.upcomingRaceList.currentItem().text()
        r1 = racing.split(' VS ')[0]
        r2 = racing.split('VS ')[1]
        self.racer1_name.setText(r1)
        self.racer2_name.setText(r2)
        # Remove the selected race from the upcomingRaceList
        self.upcomingRaceList.takeItem(self.upcomingRaceList.currentRow())
#         self.find_arduino_port()
        self.start()
        
    
    def randomly_select_all_racers(self):
        # Collect all racers from the racersList
        all_racers = [self.racersList.item(i).text() for i in range(self.racersList.count())]

        if len(all_racers) >= 2:
            # Randomly shuffle the list of racers
            random.shuffle(all_racers)

            # Select racers from the shuffled list and add them to the upcomingRaceList
            while len(all_racers) >= 2:
                racer_1 = all_racers.pop()
                racer_2 = all_racers.pop()
                self.upcomingRaceList.addItem(racer_1 + ' VS ' + racer_2)

            # Clear the racersList
            self.racersList.clear()
    
    def randomly_select_racers(self):
        # Collect all racers from the racersList
        all_racers = [self.racersList.item(i).text() for i in range(self.racersList.count())]

        if len(all_racers) >= 2:
            # Randomly shuffle the list of racers
            random.shuffle(all_racers)

            # Select racers from the shuffled list and assign them to self.racer_1 and self.racer_2
            self.racer_1 = all_racers[0]
            self.racer_2 = all_racers[1]

            # Add the selected racers to the upcomingRaceList
            self.upcomingRaceList.addItem(self.racer_1 + ' VS ' + self.racer_2)

            # Remove the selected racers from the racersList
            row_1 = self.racersList.row(self.racersList.findItems(self.racer_1, QtCore.Qt.MatchExactly)[0])
            row_2 = self.racersList.row(self.racersList.findItems(self.racer_2, QtCore.Qt.MatchExactly)[0])
            self.racersList.takeItem(row_1)
            self.racersList.takeItem(row_2)

            # Clear the racer names
            self.racer1_name.setText("")
            self.racer2_name.setText("")



    
    def delete_race(self):
        selected_items = self.upcomingRaceList.selectedItems()
        for item in selected_items:
            # Extract racers from the race text (e.g., "Racer 1 VS Racer 2")
            race_text = item.text()
            racers = race_text.split(' VS ')
            if len(racers) == 2:
                racer_1, racer_2 = racers
                # Add racers back to racersList
                self.racersList.addItem(racer_1)
                self.racersList.addItem(racer_2)
            # Remove the selected race from the upcomingRaceList
            self.upcomingRaceList.takeItem(self.upcomingRaceList.row(item))

    
    def racing(self):
        self.racersList.hide()
        self.addRacerButton.hide()
        self.loadRacerButton.hide()
        self.comboBox.hide()
        self.label.hide()
        self.upcomingRaceList.setGeometry(20, 190, 200, 300)
        self.cancel_racers.hide()
        self.ok_racers.hide()
        self.delete_race_button.hide()
        self.randomSelectButton.hide()
        self.randomAllButton.hide()
        self.race_button.hide()
        self.lcdNumber.show()
        self.lcdNumber_2.show()
        self.raceNowButton.show()
        self.arduinoLabel.show()
        self.arduinoLabel.setText('Waiting for Race')
        self.readyButton.show()  # Show the "Ready" button
        self.resetRaceButton.hide()  # Hide the "Reset Race" button initially
    
    def find_arduino_port(self):
        self.arduino_ports = [p.device
            for p in serial.tools.list_ports.comports()
            if 'COM' in p.description
        ]
        print(self.arduino_ports)
        if not self.arduino_ports:
            self.arduinoLabel.clear()
            self.arduinoLabel.setText("No Arduino found\n.Make sure it's connected.")
            raise IOError("No Arduino found. Make sure it's connected.")
        return self.arduino_ports


    def start(self):
        selected_port = self.comboBox_2.currentText()  # Get the selected COM port from the dropdown
        self.ser = serial.Serial(port=selected_port, baudrate=115200, timeout=0.4)
        self.arduinoLabel.clear()
        self.arduinoLabel.setText("Connected to\t{}\nPress Ready When Ready!".format(self.ser.port))
        self.readyButton.show()
        if self.ser.readable():
            self.read_serial_data()
        else:
            self.arduinoLabel.clear()
            self.arduinoLabel.addItem("No connection..\nCheck Arduino Connections")

    
    def read_serial_data(self):
        while self.race_started == True:
            try:
                self.text = self.ser.read_until(b'\r\n')
                self.data = (self.text.decode('utf-8').rstrip())
                print(self.data)
            except:
                pass
    
    def racers_ok(self):
        if self.racer_1 and self.racer_2:
            self.upcomingRaceList.addItem(self.racer_1 + ' VS ' + self.racer_2)
            # Clear the names from the qlabels
            self.racer_1 = ""
            self.racer_2 = ""
            self.racer1_name.setText(self.racer_1)
            self.racer2_name.setText(self.racer_2)
    
    def clear_racers(self):
        if self.racer_1 and not self.racer_2:
            # Only one racer is chosen, add it back to the racersList
            self.racersList.addItem(self.racer_1)
            self.racer_1 = ""
            self.racer1_name.setText(self.racer_1)
        elif self.racer_2 and not self.racer_1:
            # Only one racer is chosen, add it back to the racersList
            self.racersList.addItem(self.racer_2)
            self.racer_2 = ""
            self.racer2_name.setText(self.racer_2)
        else:
            # Both racers are chosen, and they won't be added back to the racersList
            self.racer_1 = ""
            self.racer_2 = ""
            self.racer1_name.setText(self.racer_1)
            self.racer2_name.setText(self.racer_2)
        
        
    def load_home_group(self):
        number_of_racers = 0
        self.racersList.clear()
        with open('{}.csv'.format(self.comboBox.currentText())) as home_file:
            reader = csv.reader(home_file)
            for row in reader:
                number_of_racers += 1
                self.racersList.addItem(str(row[0]))
    
    def add_racer(self):
        selected_item = self.racersList.currentItem()
        if selected_item:
            selected_racer = selected_item.text()
            if self.racer_1 == "":
                self.racer_1 = selected_racer
                self.racersList.takeItem(self.racersList.row(selected_item))
                self.racer1_name.setText(self.racer_1)
            elif self.racer_2 == "":
                self.racer_2 = selected_racer
                self.racersList.takeItem(self.racersList.row(selected_item))
                self.racer2_name.setText(self.racer_2)

home_groups = ['Cagney', 'Coghlan', 'Foynes', 'James', 'Lombard', 'Mannix', 'Gavin', 'Ryan', 'Simonds']
students = []


if __name__ == "__main__":
    app = QtWidgets.QApplication(sys.argv)
    window = MyWindow()
    window.show()
    sys.exit(app.exec())