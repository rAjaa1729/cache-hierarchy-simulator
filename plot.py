import matplotlib.pyplot as plt

# Define the data for each file
x_labels=[['8','16','32','64','128'],['512', '1024', '2048',
'4096', '8192'],['1','2','4','8','16'],['16384','32768','65536','131072','262144'],['1','2','4','8','16']]
titles=['access time vs Block size','access time vs L1cache Size','access time vs L1cache Assoc','access time vs L2cache size','access time vs L2cache Assoc']
plotxlabel=['Block Size','L1cache Size','L1cache Assoc','L2cache Size','L2cache Assoc']

# Open the file for reading
with open('trace_answer.txt', 'r') as f:
    # Read all lines from the file
    lines = f.readlines()

n=0
for mondal in range(1,2):
    for i in range(0,5):
        print(x_labels[i],titles[i],plotxlabel[i])
        # Split the line into a list of numeric values
        data = list(map(float, lines[n].rstrip().split(',')))
        plt.bar(x_labels[i], data)
        plt.title(titles[i])
        plt.xlabel(plotxlabel[i])
        plt.ylabel('total access time')
        n+=1
        plt.show()
