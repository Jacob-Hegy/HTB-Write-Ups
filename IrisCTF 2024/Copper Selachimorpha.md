
## IrisCTF 2024 Write-Up: Copper Selachimorpha | Jacob Hegy

![](https://cdn-images-1.medium.com/max/2000/1*2zcg4bx9ILcf5-4g6rKd5Q.png)

Hey y’all, this is a bit of a different write-up! I was part of the team “Binary Blossoms” participating in the Iris2024 CTF, and with it just wrapping up I figured I’d do a write-up on one of the problems that I enjoyed doing!

![](https://cdn-images-1.medium.com/max/2000/0*pKKOjKmKX3aTc8Dk.png)

The problem I’ll be covering is Networks #3 “Copper Selachimorpha”. The premise is as follows:

![](https://cdn-images-1.medium.com/max/2312/1*jnO07-vjv5e577thCOWOHw.png)

## Initial Investigation

The provided zip contains a packet capture of encrypted 802.11 traffic.

![](https://cdn-images-1.medium.com/max/3840/1*i2LyzotOvd77ed6I2f5s7w.png)

We’ll have to decrypt it first. To get the key, we can use aircrack-ng to perform a dictionary attack.

![](https://cdn-images-1.medium.com/max/2000/1*6AH_mvHkus9ZfdThcbGumg.png)

Once it finishes, we get the password “humus12345”. We can use this to view the encrypted traffic in Wireshark. To do so, we’ll add the password to our preferences (Edit > Preferences > Protocols > IEEE 802.11 > Edit…). The password will be of key type wpa-pwd and the format *password:ssid*.

![](https://cdn-images-1.medium.com/max/2654/1*PZ9Yklaov7gTSP34FuoBzQ.png)

Once we enter the password, a lot of the traffic decrypts and we can begin analysis.

At packet 114, we see Joe begin logging into an FTP server. Packet 121 is the server requesting Joe’s password.

![](https://cdn-images-1.medium.com/max/2234/1*Qb0InQeaCRKKmjhd2szM6Q.png)

We’ll advance along this conversation with CTRL + > to find where Joe sends his password. Doing so brings us to packet 159 which contains a partial flag.

![](https://cdn-images-1.medium.com/max/2322/1*zE-z-qlRKslDKQ1K4yV14A.png)

We’ll make note of this for later. Continuing our analysis, we find a lot of FTP and FTP-data traffic, so we’ll filter specifically for that with *ftp || ftp-data*.

![](https://cdn-images-1.medium.com/max/3834/1*dJAIH8HCSV9A6_8QicrM9w.png)

Doing so shows us Joe requesting a file entitled “beautiful_fish.png” from the FTP server. We can assume this is likely where the rest of our flag is based on the instructions. Now it’s a matter of getting the png.

## Going Fishing

First, make note of the size of the file as listed in packet 286.

![](https://cdn-images-1.medium.com/max/2000/1*USG2KD8f253-uGulahlERg.png)

PNG beautiful_fish.png is going to be 206908 bytes in size. Looking further down, we see all the FTP-data of the file being transferred.

![](https://cdn-images-1.medium.com/max/3082/1*OYAonxiCDacWqeAHZne8Pw.png)

Note each packet contains 1364 bytes of data for beautiful_fish.png. Doing the math, this will require, at minimum, 152 packets to fully transmit the file.

Something additionally worth making note of is that some of the segments say “TCP Previous segment not captured”. This may seem bad, however make note of the fact that we still have access to the FTP-data in each packet. Thus, we’ll treat these packets as normal transmissions.

There are three separate transmissions of beautiful_fish.png in the packet capture. The first begins at packet 309 and ends at packet 840, the second begins at packet 953 an ends at packet 1529, and the third begins at packet 1652 and ends at packet 2139. Great, so we can just reconstruct the file from one of these transmissions, right? Well, not quite…

![Transmission #1](https://cdn-images-1.medium.com/max/2000/1*0CjjCusoaJakJnQY9C83mg.png)

![Transmission #2](https://cdn-images-1.medium.com/max/2000/1*qj5uJzNi8rstBV33QTVuyg.png)

![Transmission #3](https://cdn-images-1.medium.com/max/2000/1*qj5uJzNi8rstBV33QTVuyg.png)

Recall earlier that our transmission should contain 152 packets at bare minimum. Each of these transfers were partial, none of them contain the full beautiful_fish.png. So what are we to do?

## Tipping the Scales

Well we’ve got three transmissions of the same file, each one incomplete. That begs the question: can we make one good transmission from the three bad ones? To answer this, we’re gonna need tshark.

TShark is a command line tool that reads packet captures in the same way that Wireshark does, but it offers a much easier way for us to mass extract data. We’re gonna use it to extract two things from the FTP-data packets: tcp.payload and tcp.seq.

Tcp.payload is self-explanatory enough, but what about tcp.seq? Well, tcp.seq contains the sequence number for each packet in the FTP transfer. The sequence number helps the receiver to order the packets as it receives them as well as notify the sender if a packet is missing due to gap in the sequence. It starts at some arbitrary number and increments by the size in bytes of the transfer each time (in this case, 1364 bytes).

What we’ll be doing is collecting the hex from all the FTP-data packets as well as their relative TCP sequence numbers (the difference between relative and raw sequence numbers is relative starts at 1), comparing the data in each sequence, and combining unique packets from each stream together into a single stream. From there, it’ll be as easy as sorting by the sequence numbers to ensure order is properly maintained, then we can just write the raw hex to a png file and open it!

So to start off, lets go ahead and extract the data with tshark:

![Extracting the hex for transmission #1](https://cdn-images-1.medium.com/max/2450/1*bc1XG1fJvA7f25fHCGL8CA.png)

![Extracting the sequence numbers for transmission #1](https://cdn-images-1.medium.com/max/2442/1*98Y18xi7fZ2KlPrOtBDFJQ.png)

![Extracting the hex for transmission #2](https://cdn-images-1.medium.com/max/2446/1*qELJiZNtK26aw-p_zNOuIA.png)

![Extracting the sequence numbers for transmission #2](https://cdn-images-1.medium.com/max/2444/1*qdt_DBomm81fxIN7zJlAkA.png)

![Extracting the hex for transmission #3](https://cdn-images-1.medium.com/max/2442/1*T8isBYXMk1rolJfcJ7wyLQ.png)

![Extracting the sequence numbers for transmission #3](https://cdn-images-1.medium.com/max/2450/1*215v-zQyVIJYY93u_K0TEg.png)

We could’ve dumped the hex and sequence numbers for each transmission stream into the same file and turned it into a .csv, but I decided to just do it in separate files because I wanted to make writing the python as easy as possible.

Moving onto the script, I used the following:

    class Stream:
     def __init__(self):
      self.hexes = []
      self.nums = []
    
    p1, p2, p3 = Stream(), Stream(), Stream()
    
    with open("stream1.txt", "r") as f:
     with open("stream1nums.txt", "r") as g:
      x = f.read()
      y = g.read()
      hexes = x.split('\n')
      nums = y.split('\n')
      hexes = hexes[:len(hexes) - 1]
      nums = nums[:len(nums) - 1]
      for i in range(len(hexes)):
       p1.hexes.append(hexes[i])
       p1.nums.append(int(nums[i]))
    
    with open("stream2.txt", "r") as f:
     with open("stream2nums.txt", "r") as g:
      x = f.read()
      y = g.read()
      hexes = x.split('\n')
      nums = y.split('\n')
      hexes = hexes[:len(hexes) - 1]
      nums = nums[:len(nums) - 1]
      for i in range(len(hexes)):
       p2.hexes.append(hexes[i])
       p2.nums.append(int(nums[i]))
    
    with open("stream3.txt", "r") as f:
     with open("stream3nums.txt", "r") as g:
      x = f.read()
      y = g.read()
      hexes = x.split('\n')
      nums = y.split('\n')
      hexes = hexes[:len(hexes) - 1]
      nums = nums[:len(nums) - 1]
      for i in range(len(hexes)):
       p3.hexes.append(hexes[i])
       p3.nums.append(int(nums[i]))
    
    i = 0
    while i < len(p2.hexes):
     if p2.hexes[i] not in p1.hexes:
      p1.hexes.append(p2.hexes[i])
      p1.nums.append(p2.nums[i])
      i = i - 1
     i = i + 1
    
    i = 0
    while i < len(p3.hexes):
     if p3.hexes[i] not in p1.hexes:
      p1.hexes.append(p3.hexes[i])
      p1.nums.append(p3.nums[i])
      i = i - 1
     i = i + 1
    
    # A bubble sort, the only sort fitting for a fish
    for i in range(len(p1.nums) - 1):
     for j in range(0, len(p1.nums) - 1 - i):
      if p1.nums[j] > p1.nums[j + 1]:
       temp = p1.nums[j]
       p1.nums[j] = p1.nums[j + 1]
       p1.nums[j + 1] = temp
       temp = p1.hexes[j]
       p1.hexes[j] = p1.hexes[j + 1]
       p1.hexes[j + 1] = temp
    
    for i in range(len(p1.nums) - 1):
     print("Current Seq Num = {} | Next Seq Num = {} | Difference = {}".format(p1.nums[i], p1.nums[i + 1], p1.nums[i + 1] - p1.nums[i]))
    
    print(len(p1.hexes))
    
    with open("fishhex.txt", "w") as f:
     for i in range(len(p1.hexes)):
      f.write(p1.hexes[i])

A quick explanation:

* I created the Stream class to contain a list containing the payloads and sequence numbers for each FTP transmission.

* Each packet is separated with a new line in the files dumped by tshark, so once I read the contents of each file it was as simple as splitting along “\n” and removing the last entry as it was just an empty space.

* Once the three streams were established, p2 and p3 were checked for hex payloads that weren’t currently in p1. If any were found, they were appended onto the end of the hexes list in p1 and their sequence number was also appended onto the nums list in p1.

* Once all packets were combined into a filestream, I used a bubble sort to sort the hexes and sequence numbers according to the sequence numbers in ascending order (you can use a much more efficient sort if you want, I just used a bubble sort for simplicity sake). This works fine because I ensured the sequence numbers for each packet were in the same index in nums as their respective payload in hexes.

* We can the check the combined filestream with the two prints statements, one checking if each packet increments its sequence number by 1364 and the other checking to see the number of packets in the combined filestream.

* Finally, the complete filestream is written, line-by-line, into the text file “fishhex.txt” for usage later.

After running the script, we’re met with the following output (I’m just showing the end of the output, if you want to see the full thing you can run the script yourself):

![](https://cdn-images-1.medium.com/max/2000/1*GNxTP2ltwhAT648RnZ1BNQ.png)

Not only do we see that each packet increments by 1364, but our combined filestream is 152 packets. That’s the same amount we determined our final file would have to be for beautiful_fish.png.

Now we just have to make the file. We can use the following script to do so:

    # dumpToFile.py
    from struct import pack
    import sys
    
    with open(sys.argv[1], 'r') as f:
     hexdump = f.read()
     form = '<' + 'B' * ((int)(len(hexdump) / 2))
     with open(sys.argv[2], "wb") as g:
      g.write(pack(form, *bytearray.fromhex(hexdump)))

Using command-line arguments, we’ll execute the script with `python dumpToFile.py fishhex.txt beautiful_fish.png.` After we run that, we can check our files and we’ll find something waiting for us:

![](https://cdn-images-1.medium.com/max/2216/1*5BAD3eAo3InXc3fck9IjfQ.png)

That’s the rest of our flag! That gives us the complete flag:
>  irisctf{welc0me_t0_th3_n3twork_c4teg0ry_we_h0pe_you_enj0yed_th3_sh4rks}

And that’s it! Congratulations to all participating teams!
