package rushhour;

import java.util.ArrayDeque;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Queue;

/**
 * The RushHourBoard is at the core of the RushHour solver. It controls setup,
 * move making, solving, and print formatting/output.
 */

public class RushHourBoard {
  // Board size, max 10x10 due to position descript string format

  int height = 6;
  int width = 6;

  // Exit location
  int exitRow = 2;
  int exitCol = 6;

  // Cars on board, static info referenced by array index from
  // board setup string
  ArrayList<Car> cars = new ArrayList<Car>();

  // Board setup description is as follows:
  // - Top left coordinate of vehicle, zero based, row col with no separating char
  // - Two-digit referenc to car (index into cars array), 00-99, leading zero required
  // IE: 01004401
  String initialSetup = "";
  String finalSetup = null;
  private HashMap<String, String> solution;
  private HashMap<String, Boolean> visited;
  private HashMap<String, String> moveToReach;

  /**
   * Create a Car and add it to this board.
   *
   * @param color
   *          The color of the car
   * @param size
   *          The size of the car
   * @param isHorizontal
   *          Whether or not the car is horizontal
   * @param initialRow
   *          The row of the car
   * @param initialCol
   *          The column of the car
   */
  public void addCar(String color, int size, boolean isHorizontal,
          int initialRow, int initialCol) {
    Car newCar = new Car(color, size, isHorizontal);
    addCar(newCar, initialRow, initialCol);
  }

  /**
   * Add an existing Car to the board.
   *
   * @param newCar
   *          The Car to be added
   * @param initialRow
   *          The row of the car
   * @param initialCol
   *          The column of the car
   */
  public void addCar(Car newCar, int initialRow, int initialCol) {
    int newIndex = cars.size();
    cars.add(newCar);

    String loc = String.format("%d%d%02d", initialRow, initialCol, newIndex);
    initialSetup = initialSetup + loc;

    // We'll need to solve again if we already had
    finalSetup = null;
  }

  /**
   * Solves the board's given setup.
   *
   * @return true if a solution is found
   */
  public boolean solve() {
    finalSetup = null;
    solution = new HashMap<String, String>();
    visited = new HashMap<String, Boolean>();
    moveToReach = new HashMap<String, String>();

    // Start at our initial setup
    Queue<String> queue = new ArrayDeque<String>();
    queue.add(initialSetup);
    solution.put(initialSetup, null);
    moveToReach.put(initialSetup, null);

    while (!queue.isEmpty()) {
      // Next set up
      String curr = queue.remove();

      // Skip board setups we've already examined
      if (visited.containsKey(curr)) {
        continue;
      }
      visited.put(curr, Boolean.TRUE);

      // Are we the solution the puzzle?
      if (isSolution(curr)) {
        finalSetup = curr;
        return true;
      }

      // Find each of the places one move away from the current position
      // This means walk through each car and move it to each of its valid
      // locations
      // Save them into the tree as well as queue them up for examination
      for (int i = 0; i < curr.length(); i += 4) {
        // Get car details
        int row = Integer.parseInt(curr.substring(i, i + 1));
        int col = Integer.parseInt(curr.substring(i + 1, i + 2));
        int index = Integer.parseInt(curr.substring(i + 2, i + 4));
        Car c = cars.get(index);

        // Check moving the car steadily further from where it started, stopping
        // moving one direction if it becomes invalid
        boolean maxedPos = false;
        boolean maxedNeg = false;
        int dist = 1;
        while (!maxedPos || !maxedNeg) {
          StringBuilder mutationPos = new StringBuilder(curr);
          StringBuilder mutationNeg = new StringBuilder(curr);

          String movePos;
          String moveNeg;

          // Find the two possible changes
          Integer newColPos = col;
          Integer newColNeg = col;
          Integer newRowPos = row;
          Integer newRowNeg = row;
          if (c.isHorizontal()) {
            newColPos = col + dist;
            newColNeg = col - dist;

            movePos = c.getColor() + " right " + dist;
            moveNeg = c.getColor() + " left " + dist;
          } else {
            newRowPos = row + dist;
            newRowNeg = row - dist;

            movePos = c.getColor() + " down " + dist;
            moveNeg = c.getColor() + " up " + dist;
          }

          // Change string coor
          mutationPos.replace(i, i + 1, newRowPos.toString());
          mutationPos.replace(i + 1, i + 2, newColPos.toString());
          mutationNeg.replace(i, i + 1, newRowNeg.toString());
          mutationNeg.replace(i + 1, i + 2, newColNeg.toString());

          // Check it, drop invalid positions
          String childPos = mutationPos.toString();
          String childNeg = mutationNeg.toString();
          if (!maxedPos && isValid(childPos)) {
            // Save how to get here if we haven't already
            // Don't want to overwite existing solution as it was
            // either faster or the same
            if (!solution.containsKey(childPos)) {
              solution.put(childPos, curr);
              moveToReach.put(childPos, movePos);
            }

            // Explore this node later
            queue.add(childPos);
          } else {
            maxedPos = true;
          }

          if (!maxedNeg && isValid(childNeg)) {
            if (!solution.containsKey(childNeg)) {
              solution.put(childNeg, curr);
              moveToReach.put(childNeg, moveNeg);
            }
            queue.add(childNeg);
          } else {
            maxedNeg = true;
          }

          // Go further from current point
          dist++;
        }
      }
    }

    // No solution found
    return false;
  }

  /**
   * Get the solution of the parent for a given location.
   *
   * @param loc
   *          The location to get the parent of
   * 
   * @return a String based solution for location
   */
  public String getParent(String loc) {
    return solution.get(loc);
  }

  /**
   * Get the move required to get to location loc
   * @param loc
   *          The location we are concerned with
   *
   * @return the move required to get to location loc
   */
  public String getMoveTo(String loc) {
    return moveToReach.get(loc);
  }

  /**
   * Get the String value that holds the initial setup of the board.
   *
   * @return the initial setup of the board
   */
  public String getInitial() {
    return initialSetup;
  }

  /**
   * Get the String value that holds the final setup of a solved board.
   *
   * @return the final setup of the board
   */
  public String getFinal() {
    // Solve if needed
    if (finalSetup == null) {
      solve();
    }

    return finalSetup;
  }

  /**
   * Determine whether or not the given location is a solution to the puzzle.
   *
   * @param loc
   *          The location to check for solution.
   *
   * @return true if a solution has been found
   */
  private boolean isSolution(String loc) {
    // Find the red car and check if it is touching the exit
    // Technically we know the red will always be the first one
    // because that's what Dr. G specified but I want to be more
    // generic
    for (int i = 0; i < loc.length(); i += 4) {
      // Get car details
      int row = Integer.parseInt(loc.substring(i, i + 1));
      int col = Integer.parseInt(loc.substring(i + 1, i + 2));
      int index = Integer.parseInt(loc.substring(i + 2, i + 4));
      Car c = cars.get(index);

      if (c.isTarget()) {
        // Other end is touching exit column
        if (col + c.getSize() == exitCol) {
          return true;
        } else {
          return false;
        }
      }
    }

    // Guess we didn't find the red car...?
    // Really this should be an exception, invalid loc string
    return false;
  }

  /**
   * Checks a given board to see if it is valid. This is done by simple drawing
   * the board onto an array. If any car overlaps (writing onto an already
   * filled location) or goes off the edge, then return false.
   *
   * @param loc
   *          The board setup string to validate
   *
   * @return true if the setup string is valid
   */
  private boolean isValid(String loc) {
    try {
      // Empty board initially
      char[][] out = new char[height][width];

      // Go through each car
      for (int i = 0; i < loc.length(); i += 4) {
        // Get car details
        int row = Integer.parseInt(loc.substring(i, i + 1));
        int col = Integer.parseInt(loc.substring(i + 1, i + 2));
        int index = Integer.parseInt(loc.substring(i + 2, i + 4));
        Car c = cars.get(index);

        // Draw on board
        for (int s = 0; s < c.getSize(); s++) {
          if (c.isHorizontal()) {
            if (col + s < out[row].length && out[row][col + s] != 'x') {
              out[row][col + s] = 'x';
            } else {
              return false;
            }
          } else {
            if (row + s < out.length && out[row + s][col] != 'x') {
              out[row + s][col] = 'x';
            } else {
              return false;
            }
          }
        }
      }
    } catch (Exception e) {
      // Error parsing string of some kind, so definitely invalid
      return false;
    }

    return true;
  }

  /**
   * Before and after board print out (if already solved).
   *
   * @return a string holding the before and after setup of the board
   */
  @Override
  public String toString() {
    StringBuilder sb = new StringBuilder("Initial setup:\n");
    sb.append(toString(initialSetup));

    if (finalSetup != null) {
      sb.append("\nFinal setup:\n");
      sb.append(toString(finalSetup));
    }

    return sb.toString();
  }

  /**
   * Pretty output of a given board setup.
   *
   * @param loc
   *          The board setup to print out
   *
   * @return a string with pretty output of board
   */
  public String toString(String loc) {
    // Empty board initially
    char[][] out = new char[height][width];
    for (int row = 0; row < height; row++) {
      for (int col = 0; col < width; col++) {
        out[row][col] = ' ';
      }
    }

    // Go through each car
    for (int i = 0; i < loc.length(); i += 4) {
      // Get car details
      int row = Integer.parseInt(loc.substring(i, i + 1));
      int col = Integer.parseInt(loc.substring(i + 1, i + 2));
      int index = Integer.parseInt(loc.substring(i + 2, i + 4));
      Car c = cars.get(index);

      // Draw on board
      for (int s = 0; s < c.getSize(); s++) {
        if (c.isHorizontal()) {
          out[row][col + s] = c.getColor().charAt(0);
        } else {
          out[row + s][col] = c.getColor().charAt(0);
        }
      }
    }

    // Create actual string
    StringBuilder sb = new StringBuilder();
    for (int i = 0; i < width + 2; i++) {
      sb.append('#');
    }
    sb.append('\n');
    for (int row = 0; row < height; row++) {
      sb.append("#");

      for (int col = 0; col < width; col++) {
        sb.append(out[row][col]);
      }

      if (row != this.exitRow) {
        sb.append("#\n");
      } else {
        sb.append(" \n");
      }
    }
    for (int i = 0; i < width + 2; i++) {
      sb.append('#');
    }
    sb.append('\n');

    return sb.toString();
  }
}
