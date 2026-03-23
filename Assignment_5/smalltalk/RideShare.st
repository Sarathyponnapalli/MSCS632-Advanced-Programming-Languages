"============================================================
  RideShare System — Smalltalk Implementation
  Course  : MSCS-632 Advanced Programming Languages
  Author  : Parthasarathi Ponnapalli
  
  Demonstrates: Encapsulation, Inheritance, Polymorphism
============================================================"

Object subclass: #Ride
    instanceVariableNames: 'rideID pickupLocation dropoffLocation distance'
    classVariableNames: ''
    poolDictionaries: ''
    category: 'RideShare'!

!Ride class methodsFor: 'instance creation'!
id: anId pickup: aPickup dropoff: aDropoff distance: aDist
    ^self new
        pvtSetId: anId
        pickup:   aPickup
        dropoff:  aDropoff
        distance: aDist! !

!Ride methodsFor: 'private'!
pvtSetId: anId pickup: aPickup dropoff: aDropoff distance: aDist
    rideID          := anId.
    pickupLocation  := aPickup.
    dropoffLocation := aDropoff.
    distance        := aDist! !

!Ride methodsFor: 'accessing'!
rideID
    ^rideID!
pickupLocation
    ^pickupLocation!
dropoffLocation
    ^dropoffLocation!
distance
    ^distance! !

!Ride methodsFor: 'operations'!
fare
    self subclassResponsibility!

rideType
    self subclassResponsibility!

rideDetails
    Transcript
        show: '  [', self rideType, ']';
        show: ' #', rideID printString;
        show: '  ', pickupLocation, ' -> ', dropoffLocation;
        show: '  ', distance printString, ' mi';
        show: '  $', (self fare roundTo: 0.01) printString;
        nl! !


"============================================================"
Ride subclass: #StandardRide
    instanceVariableNames: ''
    classVariableNames: ''
    poolDictionaries: ''
    category: 'RideShare'!

!StandardRide methodsFor: 'operations'!
fare
    ^2.50 + (1.20 * distance)!

rideType
    ^'Standard'! !


"============================================================"
Ride subclass: #PremiumRide
    instanceVariableNames: ''
    classVariableNames: ''
    poolDictionaries: ''
    category: 'RideShare'!

!PremiumRide methodsFor: 'operations'!
fare
    ^5.00 + (2.80 * distance) + 4.00!

rideType
    ^'Premium'!

rideDetails
    super rideDetails.
    Transcript
        show: '             (includes $4.00 luxury surcharge)';
        nl! !


"============================================================"
Ride subclass: #SharedRide
    instanceVariableNames: 'passengers'
    classVariableNames: ''
    poolDictionaries: ''
    category: 'RideShare'!

!SharedRide class methodsFor: 'instance creation'!
id: anId pickup: aPickup dropoff: aDropoff distance: aDist passengers: pax
    ^(self id: anId pickup: aPickup dropoff: aDropoff distance: aDist)
        passengers: pax;
        yourself! !

!SharedRide methodsFor: 'accessing'!
passengers
    ^passengers!
passengers: n
    passengers := (n max: 2)! !

!SharedRide methodsFor: 'operations'!
fare
    ^(2.00 + (1.00 * distance)) / passengers!

rideType
    ^'Shared'!

rideDetails
    super rideDetails.
    Transcript
        show: '             (', passengers printString,
              ' passengers @ $', (self fare roundTo: 0.01) printString, ' each)';
        nl! !


"============================================================"
Object subclass: #Driver
    instanceVariableNames: 'driverID name rating assignedRides'
    classVariableNames: ''
    poolDictionaries: ''
    category: 'RideShare'!

!Driver class methodsFor: 'instance creation'!
id: anId name: aName rating: aRating
    ^self new
        pvtSetId: anId
        name:     aName
        rating:   aRating! !

!Driver methodsFor: 'private'!
pvtSetId: anId name: aName rating: aRating
    driverID      := anId.
    name          := aName.
    rating        := aRating.
    assignedRides := OrderedCollection new! !

!Driver methodsFor: 'accessing'!
driverID
    ^driverID!
name
    ^name!
rating
    ^rating!
rideCount
    ^assignedRides size!
getRides
    ^assignedRides asArray!
totalEarnings
    ^assignedRides inject: 0 into: [:sum :r | sum + r fare]! !

!Driver methodsFor: 'operations'!
addRide: aRide
    assignedRides add: aRide!

getDriverInfo
    Transcript
        show: '  Driver #', driverID printString,
              ' | ', name,
              ' | Rating: ', rating printString, ' stars',
              ' | Trips: ', assignedRides size printString,
              ' | Earned: $', (self totalEarnings roundTo: 0.01) printString;
        nl.
    assignedRides isEmpty ifFalse: [
        assignedRides do: [:r | r rideDetails]]! !


"============================================================"
Object subclass: #Rider
    instanceVariableNames: 'riderID name requestedRides'
    classVariableNames: ''
    poolDictionaries: ''
    category: 'RideShare'!

!Rider class methodsFor: 'instance creation'!
id: anId name: aName
    ^self new pvtSetId: anId name: aName! !

!Rider methodsFor: 'private'!
pvtSetId: anId name: aName
    riderID        := anId.
    name           := aName.
    requestedRides := OrderedCollection new! !

!Rider methodsFor: 'accessing'!
riderID
    ^riderID!
name
    ^name!
rideCount
    ^requestedRides size!
getRides
    ^requestedRides asArray!
totalSpent
    ^requestedRides inject: 0 into: [:sum :r | sum + r fare]! !

!Rider methodsFor: 'operations'!
requestRide: aRide
    requestedRides add: aRide!

viewRides
    Transcript
        show: '  Rider #', riderID printString,
              ' | ', name,
              ' | Trips: ', requestedRides size printString,
              ' | Spent: $', (self totalSpent roundTo: 0.01) printString;
        nl.
    requestedRides isEmpty ifFalse: [
        requestedRides do: [:r | r rideDetails]]! !


"============================================================"
Object subclass: #RideSystem
    instanceVariableNames: 'drivers riders allRides nextRideID'
    classVariableNames: ''
    poolDictionaries: ''
    category: 'RideShare'!

!RideSystem methodsFor: 'initialization'!
initialize
    drivers    := Dictionary new.
    riders     := Dictionary new.
    allRides   := OrderedCollection new.
    nextRideID := 1000! !

!RideSystem methodsFor: 'adding'!
addDriver: aDriver
    drivers at: aDriver driverID put: aDriver!

addRider: aRider
    riders at: aRider riderID put: aRider! !

!RideSystem methodsFor: 'booking'!
bookStandard: rID driver: dID pickup: pu dropoff: dr distance: dist
    | ride |
    ride := StandardRide id: nextRideID pickup: pu dropoff: dr distance: dist.
    nextRideID := nextRideID + 1.
    self wireRide: ride rider: rID driver: dID.
    ^ride!

bookPremium: rID driver: dID pickup: pu dropoff: dr distance: dist
    | ride |
    ride := PremiumRide id: nextRideID pickup: pu dropoff: dr distance: dist.
    nextRideID := nextRideID + 1.
    self wireRide: ride rider: rID driver: dID.
    ^ride!

bookShared: rID driver: dID pickup: pu dropoff: dr distance: dist passengers: pax
    | ride |
    ride := SharedRide id: nextRideID pickup: pu dropoff: dr distance: dist passengers: pax.
    nextRideID := nextRideID + 1.
    self wireRide: ride rider: rID driver: dID.
    ^ride!

wireRide: aRide rider: rID driver: dID
    allRides add: aRide.
    (drivers at: dID) addRide: aRide.
    (riders  at: rID) requestRide: aRide! !

!RideSystem methodsFor: 'accessing'!
allRides
    ^allRides!
getDriver: id
    ^drivers at: id ifAbsent: [nil]!
getRider: id
    ^riders  at: id ifAbsent: [nil]!
totalRevenue
    ^allRides inject: 0 into: [:sum :r | sum + r fare]!
averageFare
    allRides isEmpty ifTrue: [^0].
    ^self totalRevenue / allRides size!
totalMiles
    ^allRides inject: 0 into: [:sum :r | sum + r distance]! !

!RideSystem methodsFor: 'display'!
printAllRides
    Transcript show: '  '; nl.
    Transcript show: '  Ride pool (', allRides size printString, ' rides) — polymorphic dispatch:'; nl.
    allRides do: [:r | r rideDetails]!

printSummary
    Transcript
        show: '  Total trips    : ', allRides size printString; nl;
        show: '  Total revenue : $', (self totalRevenue roundTo: 0.01) printString; nl;
        show: '  Average fare  : $', (self averageFare  roundTo: 0.01) printString; nl;
        show: '  Total miles   : ', (self totalMiles    roundTo: 0.1)  printString; nl! !


"============================================================
  MAIN — demonstration script
============================================================"

| sys |

Transcript nl.
Transcript show: '============================================================'; nl.
Transcript show: '  RideShare System — Smalltalk OOP Demo'; nl.
Transcript show: '  MSCS-632  Advanced Programming Languages'; nl.
Transcript show: '============================================================'; nl.

sys := RideSystem new.
sys initialize. "Explicitly calling initialize ensures compatibility across different GST environments"

sys addDriver: (Driver id: 1   name: 'Alice Johnson' rating: 4.9).
sys addDriver: (Driver id: 2   name: 'Carlos Rivera' rating: 4.7).
sys addDriver: (Driver id: 3   name: 'Priya Sharma'  rating: 4.8).

sys addRider: (Rider id: 101  name: 'Bob Chen').
sys addRider: (Rider id: 102  name: 'Diana Park').
sys addRider: (Rider id: 103  name: 'Ethan Nguyen').

sys bookStandard: 101 driver: 1  pickup: 'Peoria Station'  dropoff: 'Caterpillar HQ'  distance: 4.2.
sys bookPremium:  102 driver: 2  pickup: 'Embassy Suites'  dropoff: 'Civic Center'    distance: 2.8.
sys bookShared:   103 driver: 3  pickup: 'UIC Campus'      dropoff: 'O''Hare Airport' distance: 11.5 passengers: 4.
sys bookStandard: 101 driver: 2  pickup: 'Oak Park'        dropoff: 'Wrigley Field'   distance: 6.0.
sys bookPremium:  102 driver: 1  pickup: 'Midway Airport'  dropoff: 'The Loop'        distance: 9.3.

Transcript nl.
Transcript show: '── 1. ENCAPSULATION ─────────────────────────────────────'; nl.
Transcript show: '   Driver info (via getDriverInfo — encapsulated list):'; nl.
(sys getDriver: 1) getDriverInfo.
Transcript nl.
Transcript show: '   Rider info (via viewRides — encapsulated list):'; nl.
(sys getRider: 101) viewRides.

Transcript nl.
Transcript show: '── 2. INHERITANCE ───────────────────────────────────────'; nl.
Transcript show: '   Class hierarchy:'; nl.
Transcript show: '   Ride (abstract)'; nl.
Transcript show: '     |-- StandardRide  fare = $2.50 + $1.20/mi'; nl.
Transcript show: '     |-- PremiumRide   fare = $5.00 + $2.80/mi + $4.00'; nl.
Transcript show: '     +-- SharedRide    fare = ($2.00 + $1.00/mi) / passengers'; nl.
Transcript nl.
Transcript show: '   Each subclass inherits rideDetails from Ride,'; nl.
Transcript show: '   then calls  super rideDetails  and adds its own output:'; nl.
(sys getRider: 102) viewRides.

Transcript nl.
Transcript show: '── 3. POLYMORPHISM ──────────────────────────────────────'; nl.
Transcript show: '   Iterating  allRides  and calling  r fare  + rideDetails'; nl.
Transcript show: '   on each — correct subclass method runs automatically:'; nl.
sys printAllRides.

Transcript nl.
Transcript show: '── SYSTEM SUMMARY ───────────────────────────────────────'; nl.
sys printSummary.

Transcript nl.
Transcript show: '── ALL DRIVERS ──────────────────────────────────────────'; nl.
#(1 2 3) do: [:id |
    (sys getDriver: id) getDriverInfo.
    Transcript nl].

Transcript show: '── ALL RIDERS ───────────────────────────────────────────'; nl.
#(101 102 103) do: [:id |
    (sys getRider: id) viewRides.
    Transcript nl].

Transcript show: '============================================================'; nl.
Transcript show: '  Done.'; nl.
Transcript show: '============================================================'; nl.