
# O DevSecOps

Web applications are playing a vital part in today's digital world. Knowing how to find the
security issues laying inside your web application before its deployment and finding its
vulnerabilities before others after deployment would be the majority of your tasks as a Security
Specialist.
Moreover, almost all companies use microservice architecture as well as DevOps methodology.
Therefore in this lab, you will learn how to integrate security tools into your CI/CD pipeline and
continuously build, security test, and deploy the secure software. In simple words, how to
implement the DevSecOps CI/CD pipeline

## Task 1: Set up your environment

Create two VMs, for the Jenkins server and its agent.
a. Install Docker Engine on both VMs.
2. Isolate your Jenkins server behind a reverse proxy and access it through port 80 (e.g
Nginx Reverse Proxy).
3. You will be working on the following Maven project. Go through it and familiarize yourself
(git clone https://github.com/HamidullahMuslih/ot-lab-sdlc.git ).
Note: the project is written in Springboot (Backend) and Bootstrap (Frontend) and
already contains unit test cases.
4. Read and explain in one line sentence the Maven lifecycle. Understand commands for
each phase (e.g build, unit test, integration test, packaging and etc.) since you will need
to use them during the pipeline tasks.



### Implementation:

2. Configuration for nginx:



![](https://i.imgur.com/cBWL3A9.png)
    
![](https://i.imgur.com/JmjU965.png)


4. A Build Lifecycle is a well-defined sequence of phases, which define the order in which the goals are to be executed.
What ever stage we call, it will execute all the previous stages till the called stage.


    

![](https://i.imgur.com/6msxjuL.png)
    
    


Agent : 
We have to create ssh keys on server and send the public key to the client.
Lets have a clear screenshot of the final configurations for the next time (I spent tiiiimeee for just a small misconfiguration that I was doing)




![](https://i.imgur.com/7CWojEe.png)
<p align = "center">
  <i>Agent configuration </i>
 </p>
    
![](https://i.imgur.com/CwOH1GE.png)
<p align = "center">
  <i>Pay ATTENTION that this is the username of the user on the server, not a random one .. :))</i>
 </p>
        
![](https://i.imgur.com/0uei7aM.png)
    



## Task 2: Implement DevSecOps Pipeline

Create the below Jobs in the same sequence:

1. Create the first job to fetch and build the Maven project. Note: skip the testing in this job



### Implementation:

I created a freestyle project for each part. We have to set the github address and then run the "mvn -DskipTests install ".
This option -DskipTests, will skip the tests in this job.



![](https://i.imgur.com/rVspcuF.png)
<p align = "center">
  <i>Figure 1: Task2.1 configuration</i>
 </p>
    
![](https://i.imgur.com/5bNfsio.png)
<p align = "center">
  <i>Figure 2: Console output, fetch successful</i>
 </p>
    
![](https://i.imgur.com/f4OBI2c.png)
<p align = "center">
  <i>Figure 3: Console output, build without tests</i>
 </p>


-----------
2. Create the next job to perform the unit tests of the project.



### Implementation:

Another Freestyle project and run "mvn clean test".
Also, from now on, I changed the default workspace address for all of them to point to the first one (Task2.1's space) not to fetch github project for each of them.



    
![](https://i.imgur.com/HkbK1kw.png)
<p align = "center">
  <i>Figure 4: Task2.2 configuration</i>
 </p>
    
![](https://i.imgur.com/3VJMfOV.png)
<p align = "center">
  <i>Figure 5: Console output, unit testing</i>
 </p>



------------
3. Create the next job to perform the integration test on the project.



### Implementation:

I used this command:

```
mvn verify
```

In the Maven lifecycle there were 3 stages for integration testing:
- pre-integration-test: perform actions required before integration tests are executed. This may involve things such as setting up the required environment.
- integration-test : process and deploy the package if necessary into an environment where integration tests can be run.
- post-integration-test: perform actions required after integration tests have been executed. This may including cleaning up the environment.

So I decide to use verify that will ensure all the 3 steps ran.
- verify :run any checks to verify the package is valid and meets quality criteria.


The logs were same as the logs for the previous step (Unit testing). Although these two are different steps from life cycle, I couldn't understand what is different there (I couldn't see integration testing part for this)



![](https://i.imgur.com/f9vJq9b.png)

    


------------
4. Create the next job to perform static code analysis on the project (e.g with the
CheckStyle, PMD, FindBugs tools). Note: Read about Warnings Next Generation Plugin.
a. Show the reports and share your understanding.
b. Feed the static code analysis reports to the Violations Plugin and set the quality
gates for the number of bugs in one of the reports, validate your work to
fail/unstable the job. Note: after validation remove the quality gate restriction
since it will fail the last task of the lab.




### Implementation:

I installed "Warnings Next Generation" plugin that will let us see the output of the tests in diagrams and ..

Then inside the configuration for this freestyle project, I add "CheckStyle, PMD, FindBugs" as postbuild actions and build it.

Command I used:

```
mvn install checkstyle:checkstyle pmd:pmd findbugs:findbugs
```



![](https://i.imgur.com/ghn7Hq4.png)    
<p align = "center">
  <i>Figure : Build option Task2.4</i>
 </p>
    
![](https://i.imgur.com/jkkNg6Z.png)
<p align = "center">
  <i>Figure : CheckStyle</i>
 </p>
    
![](https://i.imgur.com/gJBJg1M.png)
<p align = "center">
  <i>Figure : PMD</i>
 </p>
    
![](https://i.imgur.com/uo6Smxb.png)
<p align = "center">
  <i>Figure : FindBugs   </i>
 </p> 


We can see the reports on the main page of this job:



![](https://i.imgur.com/9R8eLZi.png)
<p align = "center">
  <i>Figure : Overview</i>
 </p>


- Checkstyle: Checkstyle is a development tool to help programmers write Java code that adheres to a coding standard. It automates the process of checking Java code to spare humans of this boring (but important) task. This makes it ideal for projects that want to enforce a coding standard.

So, it will show us if there are any problem there:
As an example we can see:


    
![](https://i.imgur.com/muZ128w.png)
<p align = "center">
  <i>Figure : Warnings and errors   </i>
 </p>
    
![](https://i.imgur.com/aCvjzgG.png)
<p align = "center">
  <i>Figure : A sample error caught with this plugin</i>
 </p>



- FindBugs: Detects possible bugs in Java programs. Potential errors are classified in four ranks: scariest, scary, troubling and of concern. This is a hint to the developer about their possible impact or severity.



![](https://i.imgur.com/XhdssVa.png)



As we can see, there was not alot of detected errors . Lets open one of them and see:



![](https://i.imgur.com/GGH9ct8.png)



- PMD: It finds common programming flaws like unused variables, empty catch blocks, unnecessary object creation, and so forth.



![](https://i.imgur.com/DwM9YeY.png)

![](https://i.imgur.com/PrcblJ4.png)
    



As I understood, this Violation plugin depricate and "Warning generation" plugin itself has the ability to do what you want.
We can set the threshold and if we run it, it will fail


![](https://i.imgur.com/3pcmTgg.png)

![](https://i.imgur.com/rrwwLGX.png)
    



--------------------
5. Create the next job to dockerize your artifact from target/****.war and push it to the
docker hub.



### Implementation:


For this one, using bash script I create the dockerfile and then using dockerfile, tried to dockerize. Finally I pushed it to the dockerhub.



![](https://i.imgur.com/eLi6ZR8.png)
    



--------------------
6. Create the next job to deploy the docker image from the docker hub to the Jenkins agent and validate/show that you can access the web app


### Implementation:

I wanted to run the application on port 8082 so:



![](https://i.imgur.com/Zw2qp2q.png)



I could open the app:



![](https://i.imgur.com/K7xhmpU.png)





----------------------
7. Finally, chain all the jobs such that when you run the 1st job it should execute the rest.
Note: Use Post-Build Actions>Build other projects.


### Implementation:

For this one, simply create the chain like this:

Inside Task2.1 (my project name is task2.1) it would start building the Task2.2. Task2.2 would start 2.3 and so on.



![](https://i.imgur.com/hcui9vB.png)
    



And inside each task's page, we can see the upstream and downstream project:



![](https://i.imgur.com/kMkWjCC.png)




## Task 3: (Optional) I need more

1. Integrate Snyk with Jinkens and find project vulnerabilities.


### Implementation:

First install the plugin then:



![](https://i.imgur.com/ot5NCbh.png)
<p align = "center">
  <i>Sync configuration


Next in Snyk website I took the API key:
