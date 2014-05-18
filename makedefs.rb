fileout = File.new("classdefs.h", "w")
fileout.write("#ifndef CLASSDEFS_H\n")
fileout.write("#define CLASSDEFS_H\n")
fileout.write("\n");

#ACTORS
count = 0
filein = File.new("actdefs.cpp", "r")

while (line = filein.gets)

    if (name = /\/\/#(\S*)/.match(line))
		fileout.write("#define ACT_" + name[1] + " " + count.to_s + "\n")
		count += 1
	end
	
end


fileout.write("#define ACT_MAX " + count.to_s + "\n\n");

#OBJECTS
count = 0
filein = File.new("objdefs.cpp", "r")

while (line = filein.gets)

    if (name = /\/\/#(\S*)/.match(line))
		fileout.write("#define OBJ_" + name[1] + " " + count.to_s + "\n")
		count += 1
	end
	
end

fileout.write("#define OBJ_MAX " + count.to_s + "\n\n");

#CONDITIONS
count = 0
filein = File.new("conddefs.cpp", "r")

while (line = filein.gets)

    if (name = /\/\/#(\S*)/.match(line))
		fileout.write("#define COND_" + name[1] + " " + count.to_s + "\n")
		count += 1
	end
	
end

fileout.write("#define COND_MAX " + count.to_s + "\n\n");

#END

fileout.write("#endif\n");

filein.close
fileout.close

print "Makedefs successful!"