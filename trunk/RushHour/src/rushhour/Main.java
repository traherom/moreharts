package rushhour;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.Scanner;

/**
 * Main entry point to the RushHour solver.
 * 
 * October 29, 2009
 * @author Ryan Morehart, Tanner Perrien, Stephen Halwes
 */

public class Main {

  /**
   * Used to extract user input, create a RushHourBoard, and start the solver.
   *
   * @param args
   *          The user input for a given location of the input file.
   */
  public static void main(String[] args) {
    Scanner sc;
    if (args.length == 0) {
      sc = new Scanner(System.in);
    } else {
      File f = new File(args[0]);
      try {
        sc = new Scanner(f);
      } catch (FileNotFoundException ex) {
        System.err.println("File '" + args[0] + "' not found.");
        return;
      }
    }

    // Create and populate board
    RushHourBoard board = new RushHourBoard();
    int carCount = sc.nextInt();

    for (int c = 0; c < carCount; c++) {
      String type = sc.next();
      String color = sc.next();
      String dir = sc.next();
      int row = sc.nextInt() - 1;
      int col = sc.nextInt() - 1;
      board.addCar(color,
              (type.equalsIgnoreCase("car") ? 2 : 3),
              dir.equalsIgnoreCase("h"),
              row, col);
    }

    // Solve
    if (!board.solve()) {
      System.err.println("No solution found");
      return;
    }

    // Show boards and solution
    printSolution(board, board.getFinal(), false);
  }

  /**
   * Prints the solution that the RushHourBoard solver found.
   *
   * @param board
   *          The board used to solve
   * @param loc
   *          A board setup retrieved from RushHourBoard
   * @param withBoard
   *          Decides whether or not to print the entire board
   */
  public static void printSolution(RushHourBoard board, String loc, boolean withBoard) {
    String parent = board.getParent(loc);
    if (parent != null) {
      printSolution(board, parent, withBoard);

      System.out.println(board.getMoveTo(loc));
      if (withBoard) {
        System.out.println(board.toString(loc));
      }
    } else {
      System.out.println("Solution:");
      System.out.println(board.toString(board.getInitial()));
    }
  }
}
