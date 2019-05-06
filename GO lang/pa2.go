package main

/*“I Nick Poidomani NID: ni915898, affirm that this  program  is  entirely  my  own  work  and
that  I  have  neither  developed  my  code  with  any another person, nor copied
any code from any other person, nor permitted my code to be copiedor otherwise used by
any other person, nor have I copied, modified, or otherwise used programs created
by  others. I  acknowledge  that  any  violation  of  the  above  terms  will  be  treated  as
academic dishonesty.” */
import (
	"bufio"
	"fmt"
	"os"
	"strconv"
	"strings"
)

type scheduler struct {
	use       string
	upperCYL  int
	lowerCYL  int
	initCYL   int
	cylreqs   []int
	traversal int
}

func main() {
	//File to be read is the second argument
	file := os.Args[1]
	var scheduler scheduler
	scheduler = getScheduler(file)
	outputInfo(scheduler)
}
func getScheduler(filename string) (newScheduler scheduler) {
	openFile, err := os.Open(filename)
	if err != nil {
		fmt.Println("Error! The file didnt open")
	}
	fileScanner := bufio.NewScanner(openFile)
	fileScanner.Split(bufio.ScanWords)
	var scheduler scheduler
	scheduler.cylreqs = make([]int, 0)
	for fileScanner.Scan() {
		switch fileScanner.Text() {
		case "use":
			fileScanner.Scan()
			if err != nil {
				fmt.Println(" You done goofed.")
			}
			scheduler.use = strings.ToUpper(fileScanner.Text())
		case "upperCYL":
			fileScanner.Scan()
			upperCYL, err := strconv.Atoi(fileScanner.Text())
			if err != nil {
				fmt.Println(" You done goofed. ")
			}
			scheduler.upperCYL = upperCYL
		case "lowerCYL":
			fileScanner.Scan()
			lowerCYL, err := strconv.Atoi(fileScanner.Text())
			if err != nil {
				fmt.Println(" You done goofed. ")
			}
			scheduler.lowerCYL = lowerCYL
		case "initCYL":
			fileScanner.Scan()
			initCYL, err := strconv.Atoi(fileScanner.Text())
			if err != nil {
				fmt.Println(" You done goofed. ")
			}
			scheduler.initCYL = initCYL
		case "cylreq":
			fileScanner.Scan()
			cylreq, err := strconv.Atoi(fileScanner.Text())
			if err != nil {
				fmt.Println(" You done goofed. ")
			}
			scheduler.cylreqs = append(scheduler.cylreqs, cylreq)
		case "end":
			break
		}
	}
	return scheduler
}
func outputInfo(scheduler scheduler) {
	scheduler = errorCheck(scheduler)
	fmt.Printf("Seek algorithm: %s\n", scheduler.use)
	fmt.Printf("\tLower cylinder: %5d\n", scheduler.lowerCYL)
	fmt.Printf("\tUpper cylinder: %5d\n", scheduler.upperCYL)
	fmt.Printf("\tInit cylinder:  %5d\n", scheduler.initCYL)
	fmt.Printf("\tCylinder requests:\n")
	for i := 0; i < len(scheduler.cylreqs); i++ {
		fmt.Printf("\t\tCylinder %5d\n", scheduler.cylreqs[i])
	}
	switch scheduler.use {
	case "FCFS":
		runFCFS(scheduler)
	case "SCAN":
		runSCAN(scheduler)
	case "C-SCAN":
		runCSCAN(scheduler)
	case "LOOK":
		runLOOK(scheduler)
	case "C-LOOK":
		runCLOOK(scheduler)
	case "SSTF":
		runSSTF(scheduler)
	}
}
func runFCFS(scheduler scheduler) {
	//Take the requests as they have arrived and print their servicing
	scheduler.traversal = absolute(scheduler.initCYL - scheduler.cylreqs[0])
	for i := 0; i < len(scheduler.cylreqs); i++ {
		fmt.Printf("Servicing %5d\n", scheduler.cylreqs[i])
		if i <= len(scheduler.cylreqs)-2 {
			scheduler.traversal += absolute(scheduler.cylreqs[i] - scheduler.cylreqs[i+1])
		}
	}
	fmt.Printf("%s traversal count = %5d\n", scheduler.use, scheduler.traversal)
}
func runSCAN(scheduler scheduler) {
	//Order all values less than the init
	var lower []int
	lower = make([]int, 0)
	lower = getLower(scheduler)
	lower = descendingSort(lower)
	//Order all values greater than init
	var upper []int
	upper = make([]int, 0)
	upper = getUpper(scheduler)
	upper = ascendingSort(upper)
	//Merge the two upper and lower back into the cylreqs
	scheduler = merge(upper, lower, scheduler)
	if len(upper) == len(scheduler.cylreqs) {
		scheduler.traversal = absolute(upper[len(upper)-1] - scheduler.initCYL)
	} else if len(lower) == len(scheduler.cylreqs) {
		scheduler.traversal = absolute(lower[len(lower)-1] - scheduler.initCYL)
	} else {
		scheduler.traversal = absolute(scheduler.upperCYL - scheduler.initCYL)
		scheduler.traversal += absolute(scheduler.upperCYL - scheduler.cylreqs[len(scheduler.cylreqs)-1])
	}
	for i := 0; i < len(scheduler.cylreqs); i++ {
		fmt.Printf("Servicing %5d\n", scheduler.cylreqs[i])
	}
	fmt.Printf("%s traversal count = %5d\n", scheduler.use, scheduler.traversal)
}
func runCSCAN(scheduler scheduler) {
	//Order all values less than the init
	var lower []int
	lower = make([]int, 0)
	lower = getLower(scheduler)
	lower = ascendingSort(lower)
	//Order all values greater than init
	var upper []int
	upper = make([]int, 0)
	upper = getUpper(scheduler)
	upper = ascendingSort(upper)
	//Merge the two upper and lower back into the cylreqs
	scheduler = merge(upper, lower, scheduler)
	if len(upper) == len(scheduler.cylreqs) {
		scheduler.traversal = absolute(upper[len(upper)-1] - scheduler.initCYL)
	} else if len(lower) == len(scheduler.cylreqs) {
		scheduler.traversal = absolute(lower[len(lower)-1] - scheduler.initCYL)
	} else {
		scheduler.traversal = absolute(scheduler.upperCYL - scheduler.initCYL)
		scheduler.traversal += absolute(scheduler.cylreqs[len(scheduler.cylreqs)-1] + scheduler.upperCYL)
	}

	for i := 0; i < len(scheduler.cylreqs); i++ {
		fmt.Printf("Servicing %5d\n", scheduler.cylreqs[i])
	}
	fmt.Printf("%s traversal count = %5d\n", scheduler.use, int(scheduler.traversal))
}
func runLOOK(scheduler scheduler) {
	//Order all values less than the init
	var lower []int
	lower = make([]int, 0)
	lower = getLower(scheduler)
	lower = descendingSort(lower)
	//Order all values greater than init
	var upper []int
	upper = make([]int, 0)
	upper = getUpper(scheduler)
	upper = ascendingSort(upper)
	//Merge the two upper and lower back into the cylreqs
	scheduler = merge(upper, lower, scheduler)
	scheduler.traversal = absolute(scheduler.cylreqs[len(upper)-1] - scheduler.initCYL)
	scheduler.traversal += absolute(scheduler.cylreqs[len(upper)-1] - scheduler.cylreqs[len(scheduler.cylreqs)-1])
	for i := 0; i < len(scheduler.cylreqs); i++ {
		fmt.Printf("Servicing %5d\n", scheduler.cylreqs[i])
	}
	fmt.Printf("%s traversal count = %5d\n", scheduler.use, int(scheduler.traversal))
}
func runCLOOK(scheduler scheduler) {
	//Order all values less than the init
	var lower []int
	lower = make([]int, 0)
	lower = getLower(scheduler)
	lower = ascendingSort(lower)
	//Order all values greater than init
	var upper []int
	upper = make([]int, 0)
	upper = getUpper(scheduler)
	upper = ascendingSort(upper)
	//Merge the two upper and lower back into the cylreqs
	scheduler = merge(upper, lower, scheduler)
	if len(upper) == len(scheduler.cylreqs) {
		scheduler.traversal = absolute(upper[len(upper)-1] - scheduler.initCYL)
	} else if len(lower) == len(scheduler.cylreqs) {
		scheduler.traversal = absolute(lower[len(lower)-1] - scheduler.initCYL)
	} else {
		scheduler.traversal = absolute(scheduler.initCYL - upper[len(upper)-1])
		scheduler.traversal += absolute(lower[0] - upper[len(upper)-1])
		scheduler.traversal += absolute(lower[0] - lower[len(lower)-1])
	}

	for i := 0; i < len(scheduler.cylreqs); i++ {
		fmt.Printf("Servicing %5d\n", scheduler.cylreqs[i])
	}
	fmt.Printf("%s traversal count = %5d\n", scheduler.use, int(scheduler.traversal))
}
func runSSTF(scheduler scheduler) {
	var index int
	scheduler.cylreqs = append(scheduler.cylreqs, scheduler.initCYL)
	scheduler.cylreqs = ascendingSort(scheduler.cylreqs)
	for i := 0; i < len(scheduler.cylreqs); i++ {
		if scheduler.cylreqs[i] == scheduler.initCYL {
			index = i
		}
	}
	count := 0
	up := index + 1
	down := index - 1
	for i := 0; i < len(scheduler.cylreqs)-1; i++ {
		if up >= len(scheduler.cylreqs) {
			count += absolute(scheduler.cylreqs[index] - scheduler.cylreqs[down])
			index = down
			fmt.Printf("Servicing %5d\n", scheduler.cylreqs[index])
			down--
		} else if down < 0 {
			count += absolute(scheduler.cylreqs[index] - scheduler.cylreqs[up])
			index = up
			fmt.Printf("Servicing %5d\n", scheduler.cylreqs[up])
			up++
		} else if absolute(scheduler.cylreqs[index]-scheduler.cylreqs[up]) < absolute(scheduler.cylreqs[index]-scheduler.cylreqs[down]) {
			count += absolute(scheduler.cylreqs[index] - scheduler.cylreqs[up])
			fmt.Printf("Servicing %5d\n", scheduler.cylreqs[up])
			index = up
			up = index + 1
		} else {
			count += absolute(scheduler.cylreqs[index] - scheduler.cylreqs[down])
			fmt.Printf("Servicing %5d\n", scheduler.cylreqs[down])
			index = down
			down = index - 1
		}
	}
	fmt.Printf("%s traversal count = %5d\n", scheduler.use, count)
}
func errorCheck(scheduler scheduler) (newScheduler scheduler) {
	if scheduler.lowerCYL > scheduler.upperCYL {
		fmt.Printf("ABORT(13):upper (%d) < lower (%d)\n", scheduler.upperCYL, scheduler.lowerCYL)
		os.Exit(0)
	}
	if scheduler.initCYL < scheduler.lowerCYL {
		fmt.Printf("ABORT(12):initial (%d) < lower (%d)\n", scheduler.initCYL, scheduler.lowerCYL)
		os.Exit(0)
	}
	if scheduler.initCYL > scheduler.upperCYL {
		fmt.Printf("ABORT(11):initial (%d) > upper (%d)\n", scheduler.initCYL, scheduler.upperCYL)
		os.Exit(0)
	}
	for i := 0; i < len(scheduler.cylreqs); i++ {
		if scheduler.cylreqs[i] < scheduler.lowerCYL || scheduler.cylreqs[i] > scheduler.upperCYL {
			fmt.Printf("ERROR(15):Request out of bounds: req (%d) > upper (%d) or < lower (%d)\n", scheduler.cylreqs[i], scheduler.upperCYL, scheduler.lowerCYL)
			var updatedReqs []int
			updatedReqs = make([]int, 0)
			for j := 0; j < len(scheduler.cylreqs); j++ {
				if scheduler.cylreqs[j] != scheduler.cylreqs[i] {
					updatedReqs = append(updatedReqs, scheduler.cylreqs[j])
				}
			}
			scheduler.cylreqs = updatedReqs
		}
	}
	return scheduler
}
func getLower(scheduler scheduler) (array []int) {
	var lower []int
	lower = make([]int, 0)
	for i := 0; i < len(scheduler.cylreqs); i++ {
		if scheduler.cylreqs[i] <= scheduler.initCYL {
			lower = append(lower, scheduler.cylreqs[i])
		}
	}
	return lower
}
func getUpper(scheduler scheduler) (array []int) {
	var upper []int
	upper = make([]int, 0)
	for i := 0; i < len(scheduler.cylreqs); i++ {
		if scheduler.cylreqs[i] > scheduler.initCYL {
			upper = append(upper, scheduler.cylreqs[i])
		}
	}
	return upper
}
func merge(first []int, second []int, scheduler scheduler) (newScheduler scheduler) {
	for i := 0; i < len(first); i++ {
		scheduler.cylreqs[i] = first[i]
	}
	for i := 0; i < len(second); i++ {
		scheduler.cylreqs[i+len(first)] = second[i]
	}
	return scheduler
}
func ascendingSort(array []int) (sortedArray []int) {
	var temp = array
	var temp2 int
	for i := 0; i < len(array)-1; i++ {
		for j := 0; j < len(array)-i-1; j++ {
			if temp[j] > temp[j+1] {
				temp2 = temp[j]
				temp[j] = temp[j+1]
				temp[j+1] = temp2
			}
		}
	}
	return temp
}
func descendingSort(array []int) (sortedArray []int) {
	var temp = array
	var temp2 int
	for i := 0; i < len(array)-1; i++ {
		for j := 0; j < len(array)-i-1; j++ {
			if temp[j] < temp[j+1] {
				temp2 = temp[j]
				temp[j] = temp[j+1]
				temp[j+1] = temp2
			}
		}
	}
	return temp
}
func absolute(x int) (y int) {
	if x < 0 {
		return -1 * x
	}
	return x
}
