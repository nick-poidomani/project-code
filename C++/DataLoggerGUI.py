"""
Data Logger GUI for PQUAD
Group 4 SD2 Spring 2019
Alexander Meade

TO-DO:
Finish Graph Selection logic maybe swtich from combo box to choose one box
need to try and fix graphical bug

DONE:
Can take data from CSV into dataframe
can graphy using plotly locally and remotely see FilePasreTest
Can load data from csv into cells and wont overwrite
"""

import os
import wx
import wx.grid as gridlib
import matplotlib.pyplot as matplot
from pandas import pandas as pd
import numpy

# global storage vars for read in and graphing
data = pd.DataFrame()
dates = []
times = []
lightVals = []
soilVals = []
temps = []
humidity = []
header = ["Date", "Time", "Light Value", "Soil Value", "Temperature", "Humidity"]


class MyGrid(gridlib.Grid):
    graphDates = []
    graphTimes = []
    graphLights = []
    graphSoil = []
    graphTemps = []
    graphHum = []

    def __init__(self, parent):
        gridlib.Grid.__init__(self, parent, )
        self.currentDirectory = os.getcwd()
        self.gridCreated = False #usef for grid redraw
        self.fileFlag = False # use for trying to fix reopen of files


    @staticmethod
    def createGrid(self, datalist, colnames):
        #if the grid already exists redraw it with new file
        prerow = len(datalist)
        precol = len(colnames)
        if self.gridCreated:
            self.ClearGrid()
            #attempt to fix the extra column generation
            # i = 0
            # for i in range(prerow):
            #     self.DeleteRows(i)
            # for i in range(precol):
            #     self.DeleteCols(i)
            self.AppendCols(len(datalist))
            self.AppendRows(len(colnames))

        #if grid doesnt exist draw it based off file lens
        if not self.gridCreated :
            self.CreateGrid(len(datalist), len(colnames))
            if self.gridCreated == False:
                # fill in headings
                for i in range(len(colnames)):
                    self.SetColLabelValue(i, colnames[i])

                # populate the grid
                for row in range(len(datalist)):
                    for col in range(len(colnames)):
                        try:
                            self.SetCellValue(row, col, datalist[row][col])
                        except:
                            pass
            else:
                pass
        #mark that a grid has been made for subsequent file openeings
        self.gridCreated = True

# Open File Logic
    def onOpenFile(self, e):

        MG = MyGrid(self)
        wildcard = "All files (*.*)|*.*"
        fileDlg = wx.FileDialog(self, "Select Sensor Log File ", defaultDir=self.currentDirectory,
                                defaultFile="Excel Files: ", wildcard=wildcard,
                                style=wx.FD_OPEN | wx.FD_MULTIPLE | wx.FD_CHANGE_DIR)

        if fileDlg.ShowModal() == wx.ID_OK:
            paths = fileDlg.GetPaths()
            #print "Following File Selected"
            # data = paths.readlines().rstrip('\n')  # read in and strip newline chars
            # cols = []  # storage to return file at end
            for path in paths:
                #print path
                data = pd.read_csv(path, encoding='latin-1')  # open file to data frame
                data = data.replace('\n', ' ', regex=True)  # remove newlines
                data = data.dropna()  # drop all the NAs from the file


                #with pd.option_context('display.max_rows', None, 'display.max_columns', None):
                dates = data.Date
                times = data.Time
                lightVals = data.LightReading
                soilVals = data.SoilMoistureReading
                temps = data.TemperatureReading
                humidity = data.HumidityReading

        fileDlg.Destroy()
        self.Right_Panel = wx.Panel(self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL)
        RightSizer = wx.BoxSizer(wx.VERTICAL)

        self.Refresh()
        # if self.fileFlag:
        #     self.Destroy()

        self.createGrid(self, data, header) #CREATE GRID MAKES ERROR ON MULTIPLE FILE OPENS NEED TO FIGURE OUT WHY
        grid_sizer = wx.BoxSizer(wx.VERTICAL)
        grid_sizer.Add(self, 1, wx.EXPAND)
        self.Right_Panel.SetSizer(grid_sizer)
        self.Right_Panel.SetSizer(RightSizer)
        self.Right_Panel.Layout()
        RightSizer.Fit(self.Right_Panel)
        self.fileFlag = True

        for row in range(len(data)):
            for col in range(len(header)):
                # THIS LOADS DATA IN FROM THE CSV FOR THE LENGTH OF THE FRAME
                self.SetCellValue(row, 0, dates[row])
                self.SetCellValue(row, 1, times[row])
                self.SetCellValue(row, 2, str(lightVals[row]))
                self.SetCellValue(row, 3, str(soilVals[row]))
                self.SetCellValue(row, 4, str(temps[row]))
                self.SetCellValue(row, 5, str(humidity[row]))
                self.SetReadOnly(row, col, True)  # sets read only
        #redraws pictures
        self.Refresh()
        choseOneBox = wx.SingleChoiceDialog(None, 'Select type of graph', 'Graph Generation',
                                            ['Bar', 'Scatter', 'Line'])
        #, 'Stem'])

        if choseOneBox.ShowModal() == wx.ID_OK:
            # graphCombo(None, 'Graph Generation')
            # store choice in string has to be done in this scope

            graphChoice = choseOneBox.GetStringSelection()
            #print graphChoice

            #matplotlib.pyplot.bar(x, height, width=0.8, bottom=None, *, align='center', data=None, **kwargs)
            if graphChoice == 'Bar':
                choseX = wx.SingleChoiceDialog(None, 'Choose X Axis', 'X-Axis Selection',
                                                    ["Date", 'Time'])
                choseX.ShowModal()
                choiceX = choseX.GetStringSelection()

                choseY = wx.SingleChoiceDialog(None, 'Choose Y Axis', 'Y-Axis Selection',
                                                    ["Date", 'Time', 'Temperature', 'Humidity', 'Light', 'Soil Moisture'])
                choseY.ShowModal()
                choiceY = choseY.GetStringSelection()
                # the graph generation hell
                if choiceX == 'Time' and choiceY == 'Temperature':
                    matplot.bar(times,temps,label='Celsius')  # we can pass stored user input in here
                    matplot.xlabel('Time')
                    matplot.ylabel('Temperature')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Time' and choiceY == 'Soil Moisture':
                    matplot.bar(times,soilVals,label='%')  # we can pass stored user input in here
                    matplot.xlabel('Time')
                    matplot.ylabel('Soil Moisture')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Time' and choiceY == 'Light':
                    matplot.bar(times,lightVals,label='%')  # we can pass stored user input in here
                    matplot.xlabel('Time')
                    matplot.ylabel('Light')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Time' and choiceY == 'Humidity':
                    matplot.bar(times,humidity,label='Humidity %')  # we can pass stored user input in here
                    matplot.xlabel('Time')
                    matplot.ylabel('Humidity')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Time':
                    matplot.bar(times,dates)  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Time')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Temperature':
                    matplot.bar(dates, temps,label='Celsius')  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Temperature')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Light':
                    matplot.bar(dates, lightVals)  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Light')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Soil Moisture':
                    matplot.bar(dates, soilVals)  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Soil Moisture')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Humidity':
                    matplot.bar(dates, humidity,label='Humidity %')  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Humidity')
                    matplot.legend(loc='best')
                    matplot.show()
            #matplotlib.pyplot.scatter(x, y, s=None, c=None, marker=None, cmap=None, norm=None, vmin=None, vmax=None, alpha=None, linewidths=None, verts=None, edgecolors=None, *, data=None, **kwargs)[source]
            if graphChoice == 'Scatter':
                choseX = wx.SingleChoiceDialog(None, 'Choose X Axis', 'X-Axis Selection',
                                               ["Date", 'Time', 'Temperature', 'Humidity', 'Light', 'Soil Moisture'])
                choseX.ShowModal()
                choiceX = choseX.GetStringSelection()

                choseY = wx.SingleChoiceDialog(None, 'Choose Y Axis', 'Y-Axis Selection',
                                               ["Date", 'Time', 'Temperature', 'Humidity', 'Light', 'Soil Moisture'])
                choseY.ShowModal()
                choiceY = choseY.GetStringSelection()
                # the graph generation hell
                if choiceX == 'Time' and choiceY == 'Temperature':
                    matplot.scatter(times,temps,label='Celsius')  # we can pass stored user input in here
                    matplot.xlabel('Time')
                    matplot.ylabel('Temperature')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Time' and choiceY == 'Soil Moisture':
                    matplot.scatter(times,soilVals,label='%')  # we can pass stored user input in here
                    matplot.xlabel('Time')
                    matplot.ylabel('Soil Moisture')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Time' and choiceY == 'Light':
                    matplot.scatter(lightVals, times,label='%')  # we can pass stored user input in here
                    matplot.xlabel('Time')
                    matplot.ylabel('Light')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Time' and choiceY == 'Humidity':
                    matplot.scatter(times,humidity,label='Humidity %')  # we can pass stored user input in here
                    matplot.xlabel('Time')
                    matplot.ylabel('Humidity')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Time':
                    matplot.scatter(dates, times)  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Time')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Temperature':
                    matplot.scatter(dates, temps,label='Celsius')  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Temperature')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Light':
                    matplot.scatter(dates, lightVals)  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Light')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Soil Moisture':
                    matplot.scatter(dates, soilVals)  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Soil Moisture')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Humidity':
                    matplot.scatter(dates, humidity,label='Humidity %')  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Humidity')
                    matplot.legend(loc='best')
                    matplot.show()
            #matplotlib.pyplot.plot(*args, scalex=True, scaley=True, data=None, **kwargs)[source]
            if graphChoice == 'Line':
                choseX = wx.SingleChoiceDialog(None, 'Choose X Axis', 'X-Axis Selection',
                                               ["Date", 'Time', 'Temperature', 'Humidity', 'Light', 'Soil Moisture'])
                choseX.ShowModal()
                choiceX = choseX.GetStringSelection()

                choseY = wx.SingleChoiceDialog(None, 'Choose Y Axis', 'Y-Axis Selection',
                                               ["Date", 'Time', 'Temperature', 'Humidity', 'Light', 'Soil Moisture'])
                choseY.ShowModal()
                choiceY = choseY.GetStringSelection()
                #the graph generation hell
                if choiceX == 'Time' and choiceY == 'Temperature':
                    matplot.plot(times,temps, label='Celsius')  # we can pass stored user input in here
                    matplot.xlabel('Time')
                    matplot.ylabel('temperature')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Time' and choiceY == 'Soil Moisture':
                    matplot.plot(times, soilVals,label='%')  # we can pass stored user input in here
                    matplot.xlabel('Time')
                    matplot.ylabel('Soil Moisture')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Time' and choiceY == 'Light':
                    matplot.plot(times,lightVals,label='%')  # we can pass stored user input in here
                    matplot.xlabel('Time')
                    matplot.ylabel('Light')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Time' and choiceY == 'Humidity':
                    matplot.plot(times,humidity,label='Humidity %')  # we can pass stored user input in here
                    matplot.xlabel('Time')
                    matplot.ylabel('Humidity')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Time':
                    matplot.plot(dates, times)  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Time')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Temperature':
                    matplot.plot(dates, temps,label='Celsius')  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Temperature')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Light':
                    matplot.plot(dates, lightVals)  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Light')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Soil Moisture':
                    matplot.plot(dates, soilVals)  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Soil Moisture')
                    matplot.legend(loc='best')
                    matplot.show()
                if choiceX == 'Date' and choiceY == 'Humidity':
                    matplot.plot(dates, humidity,label='Humidity %')  # we can pass stored user input in here
                    matplot.xlabel('Date')
                    matplot.ylabel('Humidity')
                    matplot.legend(loc='best')
                    matplot.show()


class LoggerGUI(wx.Frame, MyGrid):
    def __init__(self, parent):
        # constructor like java
        wx.Frame.__init__(self, parent=None, title='PQuad Data Logger', size=(800, 600),
                          style=(wx.MAXIMIZE_BOX | wx.SYSTEM_MENU | wx.RESIZE_BORDER
                                 | wx.CLOSE_BOX | wx.CAPTION | wx.MINIMIZE_BOX))
        MG = MyGrid(self)  # instance of grid class
        self.currentDirectory = os.getcwd()
        self.Centre()  # opens in middle of screen

        # create menubar
        menuBar = wx.MenuBar()
        # create button
        fileButton = wx.Menu()
        #graphButton = wx.Menu()
        #runButton = wx.Menu()

        # add exit option to file button
        openItem = fileButton.Append(wx.ID_OPEN, '&Open')
        exitItem = fileButton.Append(wx.ID_EXIT, '&Close', 'Close Data Logger')
        #graphItem = graphButton.Append(wx.ID_OK, '&Graph')

        # add file button to menubar
        menuBar.Append(fileButton, 'Graph Generation')
        #menuBar.Append(graphButton, 'Graphing')
        #menuBar.Append(runButton, 'Run')

        self.SetMenuBar(menuBar)
        # bind even to press
        self.Bind(wx.EVT_MENU, MG.onOpenFile, openItem)
        self.Bind(wx.EVT_MENU, self.Quit, exitItem)


    # Quit Program Logic
    def Quit(self, e):
        # action to close with double check on button press
        yesNoBox = wx.MessageDialog(None, 'Do you wish to end program?', 'End Program', wx.YES_NO)
        yesNoAns = yesNoBox.ShowModal()
        yesNoBox.Destroy()

        if yesNoAns == wx.ID_YES:
            self.Close()


# changed to dialog so we could extra user input from it
class graphCombo(wx.Dialog):
    def __init__(self, parent, title):
        super(graphCombo, self).__init__(parent, title=title, size=(300, 250))
        # create combo box for plotting selection
        panel = wx.Panel(self)
        box = wx.BoxSizer(wx.VERTICAL)
        self.label = wx.StaticText(panel, label="Select Plotting Parameters:", style=wx.ALIGN_CENTRE)
        box.Add(self.label, 0, wx.EXPAND | wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 20)

        # creating first drop down
        selectPram = wx.StaticText(panel, label="Select Y-Axis:", style=wx.ALIGN_CENTRE)
        box.Add(selectPram, 0, wx.EXPAND | wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5)
        parameters = ["Date", 'Time', 'Temperature', 'Humidity', 'Light', 'Soil Moisture']  # all 6 choices
        self.combo = wx.ComboBox(panel, choices=parameters)

        # second drop down
        box.Add(self.combo, 1, wx.EXPAND | wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5)
        selectPram = wx.StaticText(panel, label="Select X-Axis:", style=wx.ALIGN_CENTRE)
        box.Add(selectPram, 0, wx.EXPAND | wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5)
        self.choice = wx.ComboBox(panel, choices=parameters)
        box.Add(self.choice, 1, wx.EXPAND | wx.ALIGN_CENTER_HORIZONTAL | wx.ALL, 5)
        # add spacer
        box.AddStretchSpacer()
        self.combo.Bind(wx.EVT_COMBOBOX, self.onCombo)
        self.choice.Bind(wx.EVT_CHOICE, self.onChoice)

        self.contBut = wx.Button(panel, label="Proceed")
        box.Add(self.contBut, 0, wx.ALIGN_CENTER_HORIZONTAL)
        # self.contBut.Bind(wx.EVT_CLOSE, self.OnClick()) FIGURE OUT WHY THIS DONT WORK
        box.Add(1, wx.EXPAND, 10)

        # create
        panel.SetSizer(box)
        self.Centre()
        self.Show()

    def onCombo(self, e):
        comboChoice = self.combo.GetValue()
        self.label.SetLabel("Selected " + self.combo.GetValue() + " for Y-Axis")
        yAxisChoice = self.combo.GetString()

    def onChoice(self, e):
        self.label.SetLabel("Selected " + self.choice.GetString(self.choice.GetSelection()) + "for X-Axis")
        xAxisChoice = self.combo.GetString()

    def OnClick(self, e):
        btn = e.GetEventObject().GetValue(self.combo)
        #print "Label of pressed button = ", btn


if __name__ == "__main__":
    app = wx.App()
    frame = LoggerGUI(None).Show()
    app.MainLoop()
