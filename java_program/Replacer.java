import java.lang.*;
import java.util.*;
import java.io.*;
import java.nio.charset.*;
import java.nio.file.*;

public class Replacer {

    private Charset charset;
    private Path phrasePath;
    private Path mainPath;
    private Path outputPath;
    private String mainContents;
    private String phraseContents;
    private String [] phrases;

    // constructor
    public Replacer(String mFileName, String pFileName, String oFileName) throws IOException {
        charset = StandardCharsets.UTF_8;
        phrasePath = Paths.get(pFileName);
        mainPath = Paths.get(mFileName);
        outputPath = Paths.get(oFileName);

        phraseContents =  new String(Files.readAllBytes(phrasePath), charset);
        mainContents =  new String(Files.readAllBytes(mainPath), charset);

        phrases = phraseContents.split(",");
    }

    public void replacePhrases() {
        for (int i = 0; i < phrases.length; i += 2) {
            mainContents = mainContents.replaceAll(phrases[i], phrases[i + 1]);
        }
    }

    public void deletePhrases() {
        for (String phrase : phrases) {
            mainContents = mainContents.replaceAll(phrase, "");
        }
    }

    public void writeOut() throws IOException {
        Files.write(outputPath, mainContents.getBytes(charset));
    }

    public void printPhrases() {
        for(String phrase : phrases) {
            System.out.println(phrase);
        }
    }

    public static void main(String[] args) throws IOException {
        if (args.length < 3) {
            System.out.println("Usage: java Replacer <mainFile> <phraseFile> <outputFile>");
            System.exit(0);
        }

        for(String arg : args) {
            System.out.println(arg);
        }

        Replacer pf = new Replacer(args[0], args[1], args[2]);

        pf.printPhrases();

        pf.replacePhrases();

        pf.writeOut();

    }
}

