/* “I Nick Poidomani(ni915898)afﬁrmthatthis program is entirely my own work and that I have neither developed my code
together with any another person, nor copied any code from any other person, nor permitted my code to be
copied or otherwise used by any other person, nor have I copied, modiﬁed, or otherwise used programs
 createdbyothers. Iacknowledgethatanyviolationoftheabovetermswillbetreatedasacademic dishonesty.” */

package main

import (
	"bufio"
	"fmt"
	"os"
	"strconv"
)

type schedule struct {
	processCount   int
	runFor         int
	algorithm      string
	quantum        int
	processList    []process
	readyQueue     []process
	currentProcess process
}

type process struct {
	name       string
	arrival    int
	burst      int
	burst2     int
	waitTime   int
	turnaround int
	running    bool
	complete   bool
}

func main() {

	file := os.Args[1]
	fileOut := os.Args[2]

	var scheduler schedule
	scheduler = getScheduler(file)
	scheduler.processList = getProcesses(file)

	switch scheduler.algorithm {
	case "fcfs":
		firstComeFirstServed(scheduler, fileOut)
	case "sjf":
		shortestJobFirst(scheduler, fileOut)
	}

}

//Gets the processes from a file and returns the list of processes.
func getProcesses(fileName string) (processes []process) {

	var newProcess process
	openFile, err := os.Open(fileName)

	if err != nil {
		fmt.Println("fuck! the file didnt open")
	}

	fileScanner := bufio.NewScanner(openFile)
	fileScanner.Split(bufio.ScanWords)

	processList := make([]process, 0)

	for fileScanner.Scan() {
		switch fileScanner.Text() {
		case "name":
			fileScanner.Scan()
			newProcess.name = fileScanner.Text()
		case "arrival":
			fileScanner.Scan()
			arrival, err := strconv.Atoi(fileScanner.Text())
			if err != nil {
				fmt.Println("You done goofed. ")
				fmt.Println(fileScanner.Text())
			}
			newProcess.arrival = arrival
		case "burst":
			fileScanner.Scan()
			burst, err := strconv.Atoi(fileScanner.Text())
			if err != nil {
				fmt.Println("You done goofed.")
			}
			newProcess.burst = burst
			newProcess.burst2 = burst
			processList = append(processList, newProcess)
		case "end":
			break
		}

	}
	openFile.Close()
	return processList
}

func getScheduler(fileName string) (newScheduler schedule) {

	openFile, err := os.Open(fileName)

	if err != nil {
		fmt.Println("fuck! the file didnt open")
	}

	fileScanner := bufio.NewScanner(openFile)
	fileScanner.Split(bufio.ScanWords)

	var scheduler schedule
	// var processList := make([]process, 0)

	for fileScanner.Scan() {
		switch fileScanner.Text() {
		case "processcount":
			fileScanner.Scan()
			count, err := strconv.Atoi(fileScanner.Text())
			if err != nil {
				fmt.Println(" You done goofed.")
			}
			scheduler.processCount = count
		case "runfor":
			fileScanner.Scan()
			runfor, err := strconv.Atoi(fileScanner.Text())
			if err != nil {
				fmt.Println(" You done goofed. ")
			}
			scheduler.runFor = runfor
		case "use":
			fileScanner.Scan()
			algorithm := fileScanner.Text()
			if err != nil {
				fmt.Println(" You done goofed. ")
			}
			scheduler.algorithm = algorithm
		case "quantum":
			fileScanner.Scan()
			quantum, err := strconv.Atoi(fileScanner.Text())
			if err != nil {
				fmt.Println(" You done goofed. ")
			}
			scheduler.quantum = quantum
		case "end":
			break
		}
	}
	openFile.Close()
	return scheduler
}

func firstComeFirstServed(scheduler schedule, fileOutName string) {
	fileOutput, _ := os.Create(fileOutName)

	fmt.Fprintf(fileOutput, "%3d processes\n", scheduler.processCount)
	fmt.Fprintf(fileOutput, "Using First-Come First-Served\n")
	for i := 0; i < scheduler.processCount-1; i++ {

		if scheduler.processList[i].arrival > scheduler.processList[i+1].arrival {
			temp := scheduler.processList[i]
			scheduler.processList[i] = scheduler.processList[i+1]
			scheduler.processList[i+1] = temp
		}
	}

	scheduler.readyQueue = make([]process, 0)
	currentRunIdx := 0
	for i := 0; i <= scheduler.runFor-1; i++ {
		//See if the process has arrived yet
		for j := 0; j < len(scheduler.processList); j++ {

			if scheduler.processList[j].arrival == i {
				scheduler.readyQueue = append(scheduler.readyQueue, scheduler.processList[j])
				fmt.Fprintf(fileOutput, "Time %3d : %s arrived\n", i, scheduler.processList[j].name)
			}
		}

		if scheduler.readyQueue[currentRunIdx].burst == 0 {
			fmt.Fprintf(fileOutput, "Time %3d : %s finished\n", i, scheduler.readyQueue[currentRunIdx].name)
		}

		for k := 0; k < len(scheduler.readyQueue); k++ {
			if isWaiting(scheduler.readyQueue) == true && scheduler.readyQueue[k].complete == false && scheduler.readyQueue[k].arrival <= i {
				scheduler.readyQueue[k].running = true
				currentRunIdx = k
				scheduler.readyQueue[k].waitTime = i - scheduler.readyQueue[k].arrival
				fmt.Fprintf(fileOutput, "Time %3d : %s selected (burst %3d)\n", i, scheduler.readyQueue[k].name, scheduler.readyQueue[k].burst)
			}
		}
		if isWaiting(scheduler.readyQueue) == true {
			fmt.Fprintf(fileOutput, "Time %3d : Idle\n", i)
		}
		scheduler.readyQueue[currentRunIdx].burst--
		if scheduler.readyQueue[currentRunIdx].burst <= 0 {
			scheduler.readyQueue[currentRunIdx].complete = true
			scheduler.readyQueue[currentRunIdx].running = false
			scheduler.readyQueue[currentRunIdx].turnaround = scheduler.readyQueue[currentRunIdx].waitTime + scheduler.readyQueue[currentRunIdx].burst2
		}

	}
	for i := 0; i < len(scheduler.readyQueue)-1; i++ {
		for j := 0; j < len(scheduler.processList)-i-1; j++ {
			if scheduler.readyQueue[j].name > scheduler.readyQueue[j+1].name {
				temp := scheduler.readyQueue[j]
				scheduler.readyQueue[j] = scheduler.readyQueue[j+1]
				scheduler.readyQueue[j+1] = temp
			}
		}

	}
	fmt.Fprintf(fileOutput, "Finished at time %3d\n\n", scheduler.runFor)
	for i := 0; i < scheduler.processCount; i++ {
		fmt.Fprintf(fileOutput, "%s wait %3d turnaround %3d\n", scheduler.readyQueue[i].name, scheduler.readyQueue[i].waitTime, scheduler.readyQueue[i].turnaround)
	}

}

func shortestJobFirst(scheduler schedule, fileOutName string) {
	fileOutput, _ := os.Create(fileOutName)
	fmt.Fprintf(fileOutput, "%3d processes\n", scheduler.processCount)
	fmt.Fprintf(fileOutput, "Using preemptive Shortest Job First\n")

	scheduler.readyQueue = make([]process, 0)
	currentRunIdx := 0
	for i := 0; i <= scheduler.runFor-1; i++ {

		for m := 0; m < len(scheduler.readyQueue); m++ {
			if scheduler.readyQueue[m].arrival <= i && scheduler.readyQueue[m].running == false && scheduler.readyQueue[m].complete == false {
				scheduler.readyQueue[m].waitTime++
			}
		}

		for j := 0; j < len(scheduler.processList); j++ {

			if scheduler.processList[j].arrival == i {
				scheduler.readyQueue = append(scheduler.readyQueue, scheduler.processList[j])
				fmt.Fprintf(fileOutput, "Time %3d : %s arrived\n", i, scheduler.processList[j].name)

				for n := 0; n < len(scheduler.readyQueue); n++ {
					if scheduler.readyQueue[n].running == true {
						if scheduler.readyQueue[n].burst > scheduler.processList[j].burst2 {
							scheduler.readyQueue[n].running = false
							scheduler.readyQueue[len(scheduler.readyQueue)-1].running = true
							currentRunIdx = len(scheduler.readyQueue) - 1
							fmt.Fprintf(fileOutput, "Time %3d : %s selected (burst %3d)\n", i, scheduler.readyQueue[len(scheduler.readyQueue)-1].name, scheduler.readyQueue[len(scheduler.readyQueue)-1].burst)
						}
					}
				}
			}
		}

		if scheduler.readyQueue[currentRunIdx].burst == 0 {
			scheduler.readyQueue[currentRunIdx].running = false
			scheduler.readyQueue[currentRunIdx].complete = true
			scheduler.readyQueue[currentRunIdx].turnaround = scheduler.readyQueue[currentRunIdx].waitTime + scheduler.readyQueue[currentRunIdx].burst2
			fmt.Fprintf(fileOutput, "Time %3d : %s finished\n", i, scheduler.readyQueue[currentRunIdx].name)

		}

		for k := 0; k < len(scheduler.readyQueue); k++ {
			if isWaiting(scheduler.readyQueue) == true && scheduler.readyQueue[k].complete == false && scheduler.readyQueue[k].arrival <= i && findShortProc(scheduler.readyQueue) == scheduler.readyQueue[k].name {
				scheduler.readyQueue[k].running = true
				currentRunIdx = k

				fmt.Fprintf(fileOutput, "Time %3d : %s selected (burst %3d)\n", i, scheduler.readyQueue[k].name, scheduler.readyQueue[k].burst)
			}

		}

		if isWaiting(scheduler.readyQueue) == true {
			fmt.Fprintf(fileOutput, "Time %3d : Idle\n", i)
		}

		scheduler.readyQueue[currentRunIdx].burst--
		if scheduler.readyQueue[currentRunIdx].burst <= 0 {
			scheduler.readyQueue[currentRunIdx].complete = true
			scheduler.readyQueue[currentRunIdx].running = false
			scheduler.readyQueue[currentRunIdx].turnaround = scheduler.readyQueue[currentRunIdx].waitTime + scheduler.readyQueue[currentRunIdx].burst2
		}
	}

	//sort and print wait times and turnaround

	for i := 0; i < len(scheduler.readyQueue)-1; i++ {
		for j := 0; j < len(scheduler.processList)-i-1; j++ {
			if scheduler.readyQueue[j].name > scheduler.readyQueue[j+1].name {
				temp := scheduler.readyQueue[j]
				scheduler.readyQueue[j] = scheduler.readyQueue[j+1]
				scheduler.readyQueue[j+1] = temp
			}
		}
	}
	fmt.Fprintf(fileOutput, "Finished at time %3d\n\n", scheduler.runFor)
	for i := 0; i < scheduler.processCount; i++ {
		fmt.Fprintf(fileOutput, "%s wait %3d turnaround %3d\n", scheduler.readyQueue[i].name, scheduler.readyQueue[i].waitTime, scheduler.readyQueue[i].turnaround)
	}
}

//returns true if all the processes are complete
func isFinished(queue []process) (complete bool) {
	for i := 0; i < len(queue); i++ {
		if queue[i].complete == false {
			return false
		} else {
			return true
		}
	}
	return false
}

//returns true if no processes in the list are running
func isWaiting(queue []process) (complete bool) {
	for i := 0; i < len(queue); i++ {
		if queue[i].running == true {
			return false
		}
	}
	return true
}

func findShortProc(queue []process) (index string) {

	var temp = queue
	if len(queue) == 1 {
		return queue[0].name
	}
	var temp2 process
	for i := 0; i < len(queue)-1; i++ {
		for j := 0; j < len(queue)-i-1; j++ {
			if temp[j].burst > temp[j+1].burst {
				temp2 = temp[j]
				temp[j] = temp[j+1]
				temp[j+1] = temp2
			}
		}
	}
	for j := 0; j < len(queue); j++ {
		if temp[j].complete == false {
			return temp[j].name
		}
	}
	return queue[0].name
}
