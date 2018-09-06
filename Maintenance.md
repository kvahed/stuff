# Major **Maintenance** rework #

* Not exactly a feature in traditional sense
* Customer experience should be in some cases different
* Detailed presentation, of **how exactly** and **what specifically** works is recorded

## What is it and how does it work? ##
* Data back-end of the cluster consists of **individual database servers**
* Some **mechanism** has to work on any such database server to handle local management of databases, collections, indexes, views ...  
* **Monitors agency** for changes (coming from coordinators and supervision)
* **Enacts local changes** that become necessary as a consequence
* reports to success / failure to agency, where the contractors are waiting for feedback


## What has changed and why I would still consider it a feature ##
* As a matter of being able to build a cluster within **limited time** was initially coded in `JS`.
* After the natural life cycle through bug fixes stable recently
* So why did we redo it all?
  * Had always been complicated to debug - `feature++`
  * Some recent customer experiences had shown the limitations - `feature++`
* All is done asynchronously now
* Priority lane for fast actions `feature++`

## Some experiences ##
* Accenture test (249 collections, 5000+ indexes):  
Tests run by support/QA show significant faster run time `16x`

* pokec dataset with 50% out of sync db servers recovers on `c12/c13` i.e. 2x 32 core machine machine significantly faster from restart `~5x` (network maxed out)  
