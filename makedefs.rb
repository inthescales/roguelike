filein = File.new("actdefs.cpp", "r")
fileout = File.new("classdefs.h", "w")
count = 0

fileout.write("#ifndef CLASSDEFS_H\n")
fileout.write("#define CLASSDEFS_H\n");
fileout.write("\n");

while (line = filein.gets)

    if (name = /\/\/#(\S*)/.match(line))
		fileout.write("#define ACT_" + name[1] + " " + count.to_s + "\n")
		count += 1
	end
	
end

fileout.write("\n");
fileout.write("#endif\n");

filein.close
fileout.close

print "Makedefs successful!"