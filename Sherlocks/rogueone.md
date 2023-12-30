
# Hack the Box Sherlock Write-Ups: RogueOne | Jacob Hegy

![](https://cdn-images-1.medium.com/max/2000/1*SmA6HZYBhiFpdhib79gMLA.png)

Diving back in with another Sherlock! This time around, we’re taking a look at RogueOne, one of HackTheBox’s latest.

The premise is as follows:
>  Your SIEM system generated multiple alerts in less than a minute, indicating potential C2 communication from Simon Stark’s workstation. Despite Simon not noticing anything unusual, the IT team had him share screenshots of his task manager to check for any unusual processes. No suspicious processes were found, yet alerts about C2 communications persisted. The SOC manager then directed the immediate containment of the workstation and a memory dump for analysis. As a memory forensics expert, you are tasked with assisting the SOC team at Forela to investigate and resolve this urgent incident.

We’re given a .mem file containing a RAM dump to start rummaging through. So without further delay, let’s get into it!

## Table of Contents:
* [Checking Out the Memory](#checking-out-the-memory) (Tasks 1 & 2)
* [Putting Together the Details](#putting-together-the-details) (Tasks 3-6)
* [The Nitty-Gritty](#the-nitty-gritty) (Task 7)
* [Who?](#who)
* [What Now?](#what-now)
* [Conclusion](#conclusion)

## Checking Out the Memory

Before we kick things off, we need to get a bit of info on the memory dump. Running a quick “file 20230810.mem” tells us that we’re working with a Windows machine.

![](https://cdn-images-1.medium.com/max/2000/1*-FBaf8b-cDEX0C3d2qtEZQ.png)

That’ll give us an idea of what tools we’ll be able to use to dissect the data.

After establishing the OS, I went ahead and tossed the .mem into Volatility. Volatility Framework is an open-source digital forensics framework that’s great for analyzing volatile memory dumps. It has a large suite of plugins that will be super useful for us!

To start things off, we’ll see if we can garner any additional, relevant information. Given that we’re working with a windows machine, we’ll use the windows.info plugin.

![](https://cdn-images-1.medium.com/max/2382/1*QLTlZ7jOPb9n_BfpDNY12g.png)

We get a bit from this, but nothing super relevant. We’ll go ahead and move right along to look for IOCs.

Based on the information regarding C2 communication, I’d like to take a look at the network connections that were active at the time of the dump. So we’ll go ahead and use the windows.netstat.NetStat plugin with volatility and see what we get.

![](https://cdn-images-1.medium.com/max/2244/1*YJ5AhwvYDKs5_bJlx56gPA.png)

There’s quite the slew of connections listed for us, most of which just being unestablished connections waiting for someone to connect with them. However, the ones worth making note of are the top seven. We have five different connections to a foreign TCP 443 (possibly just being web servers running HTTPS) one connection to a foreign TCP 80 (once again, likely a foreign web server, though this one is running HTTP), and finally one connection to a foreign TCP 8888.

We can likely disregard the three connections labeled as “CLOSE_WAIT” as that means communication with the foreign host had concluded at the time of the memory dump, so that leaves us with four. HTTPS and HTTP are both commonly used protocols for C2 communication, so our theory seems decent so far.

An important thing to make note of is the processes’ owner: svchost.exe. What is svchost? Service Host (svchost.exe) is a Windows process that handles DLLs (dynamic link libraries) that the computer needs to execute. Windows is actually incapable of running DLLs on its own, hence the need for svchost as a launcher. You’ll likely see it on your own system if you’re running Windows.

An important detail to make note of, however, is that malware developers like to spoof legitimate processes like svchost.exe and often name their software after it in an attempt to mask as a legitimate process. Thus, it is important to verify if the processes we’re seeing are real. Thus, we’ll check

Let’s investigate the four remaining processes to see if we can dig up any more relevant info. For this, we’ll use the windows.pslist.PsList plugin along with grep -E “6136|6812|3404”.

![](https://cdn-images-1.medium.com/max/2084/1*Lg1vCE1TlxDhG7djUt5bKA.png)

This sets off some immediate red flags. Process 6812 is the parent process of a cmd.exe process. This all but confirms that we’ve found the malicious process, but just to be safe we’ll perform an additional check of pslist looking for the parent processes 7436 and 788.

![](https://cdn-images-1.medium.com/max/2000/1*hZ1Jlv0ibhZdRTTNLG2VJA.png)

We get a ton of svchost.exe processes, all with parent proces 788: services.exe.

![](https://cdn-images-1.medium.com/max/2060/1*7yWdoxbbDaf21KHuwbOZ5g.png)

This much is to be expected as services.exe is the Service Control Manager for the operating systems. It handles the process of starting, stopping, and interacting with services, so it’s only natural for it to be the parent of svchost.exe as they’ll be created as necessary for certain services. However, there’s an anomaly. Our svchost.exe with PID 6812 has a different parent process from all the others.

![](https://cdn-images-1.medium.com/max/2062/1*50-C8o8va8lW8vmdgoxa7Q.png)

The executable explorer.exe is the file explorer in Windows, so what this means is the svchost.exe that we saw at process 6812 is not a real svchost. It was an executable run from the file explorer, not spawned by the service control manager.

That, to me, clears up any doubt and leads me to conclude that process 6812 is the malicious process.
>  Task 1 Answer: 6812

Additionally, we know that process 4364 is the malicious process’s child process.
>  Task 2 Answer: 4364

## Putting Together the Details

Now that we’ve established which was the malicious process, we need to begin documenting details. For us, that starts with the actual executable. We’ll extract it from the memory dump with the windows.dumpfile plugin, specifically targeting PID 6812 (the id of our original malicious process).

![](https://cdn-images-1.medium.com/max/2000/1*mI43ss4qUm5igODRh7Abzg.png)

The results of our command dumps a multitude of files into our present working directory (if you want, you can avoid the clutter by making a directory for the dump and specify it by adding the flag -o <file path> to our prior command), but the one we’re concerned with is the one for svchost.exe.

As we can see, the DataSectionObject unsuccessfully dumped, but we still got the ImageSectionObject. Don’t fret over the failed dump, it’s actually somewhat expected. A DataSectionObject is meant for data items while an ImageSectionObject pertains to executables. However, I’m no definitive authority on this information, so I encourage you to learn more by perusing [Microsoft’s official documentation](https://learn.microsoft.com/en-us/windows-hardware/drivers/ddi/wdm/ns-wdm-_section_object_pointers)!

But anyways, we’ll go ahead and calculate the MD5 hash for the ImageSectionObject associated with svchost.exe.

![](https://cdn-images-1.medium.com/max/2000/1*Zw2xPiDLeVRnfdTjlYS6Jw.png)
>  Task 3 Answer: 5bd547c6f5bfc4858fe62c8867acfbb5

Now that we’ve submitted the executable’s hash to the threat hunting team, we need to get a few more details down. Specifically, we want the *where* and *when *of the attack. Luckily for us, we already have that!

Jumping back to our netstat scan from earlier, we can see the details we’re looking for.

![](https://cdn-images-1.medium.com/max/2232/1*h_y11yey1B1fS9VQ1xeJ8g.png)

We can see on August 10, 2023, a foreign host at IP address 13.127.155.166 connected over TCP 8888 to local host 172.17.79.131 on TCP 64254, beginning, marking the beginning of the incident which we’ve been investigating. This malicious process was located at memory offset 0x9e8b87762080.
>  Task 4 Answer: 13.127.155.166:8888

>  Task 5 Answer: 10/08/2023 11:30:03

>  Task 6 Answer: 0x9e8b87762080

Our final step is checking out [VirusTotal](https://www.virustotal.com/gui/home/search) to see if the hash of our executable has been submitted before and if we can get any further information from the database. Sure enough, submitting the hash lands us some additional details.

![](https://cdn-images-1.medium.com/max/3314/1*jivKA96xVrwGGSsCQlYnSw.png)

A quick jump over to our details tab provides us with the information that we need.

![](https://cdn-images-1.medium.com/max/3154/1*DZ9v8nfDJ0Z_7YPyCH37EQ.png)

The executable was first submitted on August 10, 2023 at 11:58:10 UTC.
>  Task 7 Answer: 10/08/2023 11:58:10

**Similar to my other write-ups, if you’re just here for the answers you can stop here. For those of you sticking around, we’re now going to take a deeper dive into the details such as the MITRE ATT&CK Matrix’s classification of the TTPs followed by the attacker as well as what insight that can give us into who we may be dealing with. Additionally, we’ll briefly cover the appropriate actions we should follow in response to this attack to avoid further incidents.**

## The Nitty-Gritty

The security vendors’ analysis fits the behavior we observed by the target machine and attacker. The file is marked as a trojan/basic shellcode, falling under the MITRE ATT&CK Matrix’s T1059 technique, “Command and Scripting Interpreter”. In particular, it falls under the sub-technique T1059.003 “Windows Command Shell” due to its usage of cmd.exe.

![](https://cdn-images-1.medium.com/max/2762/1*TcPdL6YmSxK8Ba9t1UOcJg.png)

As far as how the executable found its way onto Simon’s machine, that remains up in the air. A decent hypothesis would be something such as an attachment on a phishing email (T1566) given that it currently represents the most common cyber attack or even a malicious file served up off the site being hosted on TCP 8888 of the attacker’s domain, but we cannot say with certainty what exactly happened prior to the conclusion of a full investigation.

Once the attachment was downloaded on the system, something resulted in its execution, possibly user interaction. Once executed, the attacker had a foothold and began its command and control stage by sending HTTP traffic between itself and the victim to communicate with the malicious svchost.exe. This led to the spawning of the child process that made use of cmd.exe to execute commands remotely on the victim workstation.

## Who?

The malware that we identified on our victim workstation is not all that complicated or advanced. Based on VirusTotal, it was first made back in 2010, suggesting that this is not an original creation by our adversary.

![](https://cdn-images-1.medium.com/max/2000/1*yUbc5Dfqsspritd6knpsEw.png)

So you might be asking “Why is any of this important? How is any of this relevant?” Well, these details allow us to begin profiling our attacker. This isn’t so much a part of the incident response process as it is in threat hunting, but it doesn’t hurt to improve our observational abilities.

Based on the simplicity of the malware and the position of Forela as a business start-up, we can likely assume we’re not dealing with a state-sponsored APT [advanced persistent threat]. Additionally, given that this payload came from the outside, it’s unlikely that this was an insider threat, though that shouldn’t be completely discounted.

Instead, the most likely attacker is probably either a financially motivated cyber criminal, a script kiddie, or a hacktivist (in that order of likelihood). Of course, conclusions cannot be cemented until the investigation into the incident is complete.

## What Now?

Now comes the all important recovery stage of the incident response process. Simply stopping an attack is not enough, steps must be taken to prevent such an incident from recurring. So what can we do? Well let’s consult MITRE once again for some recommend remediations based on our identified threat as well as rely on our own experience to make the right calls.

* Add the malware’s signature to our antivirus/antimalware softwares as well as install such programs on appropriate machines (M1049).

* Implement code signing to prevent the execution of unsigned, potentially malicious executables/scripts (M1045).

* Block all traffic from the IP address 13.127.155.166 with security controls on the network perimeter. Additionally, implementing new rules to internal network security controls (such as the yara rules found on VirusTotal for the malware) will enable the detection of similar malicious attachments should the attacker reattempt the attack from a different IP address.

* Fully rebuild the affected workstation either from the my recent sanitary image or from scratch. Prior to doing so, a full forensic copy of the hard drive should be created as to preserve evidence and enable investigation into possible compromises of confidentiality or integrity.

* Inspect machines connected to Simon’s workstation for IOCs to determine the full scope of the incident. Any additionally compromised systems should be forensically imaged and rebuilt the same as the workstation at ground zero.

* If deemed necessary, contact law enforcement as well as the involved stakeholders to inform them of the attack. Due to the lack of information on Simon’s position within the company we cannot say for sure who in all could’ve possibly been affected, so further inquiry is required.

* If permitted, share a details about the adversarial traffic and malware with threat feeds as to propagate the information.

* Complete a full report for the incident and have a debrief/lessons learned meeting with the IT and CSIRT teams to discuss the details determined through the course of the investigation as well as how well the current incident response plan performed when exposed to a real adversary.

Additional steps can be taken by the company as deemed necessary by higher ups and its internal security team.

## Conclusion

This was definitely a fun sherlock! It was a good opportunity to get my hands dirty with Volatility as well as become more familiar with the functionality and characteristics common Windows processes. I’d recommend it to anyone looking to start working with digital forensics, particularly with Windows machines.

That’s all for me for now! I’ll be back with some more write-ups soon, so stay tuned!
