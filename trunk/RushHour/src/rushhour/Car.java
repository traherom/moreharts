package rushhour;

/**
 * Car provides getters and setters for the RushHourBoard.
 */
public class Car {

  private String color;
  private int size;
  private boolean isHorizontal;

  /**
   * Constructor.
   *
   * @param color
   *          The color of the car
   * @param size
   *          The size of the car
   * @param isHorz
   *          True if the car is horizontal
   */
  public Car(String color, int size, boolean isHorz) {
    this.color = color;
    this.size = size;
    this.isHorizontal = isHorz;
  }

  /**
   * Get the size of the car.
   *
   * @return size of the car
   */
  public int getSize() {
    return size;
  }

  /**
   * Get the color of the car.
   *
   * @return the color of the car
   */
  public String getColor() {
    return color;
  }

  /**
   * Tell whether or not this is the target car.
   *
   * @return true if this car is the target
   */
  public boolean isTarget() {
    return color.equalsIgnoreCase("red");
  }

  /**
   * Tell if this car is horizontal.
   *
   * @return true if this car is horizontal
   */
  public boolean isHorizontal() {
    return isHorizontal;
  }

  /**
   * Tell if this car is vertical.
   *
   * @return true if this car is vertical
   */
  public boolean isVertical() {
    return !isHorizontal;
  }
}
