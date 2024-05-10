# Paragon

Paragon is a visual traceroute implementation designed to provide a clear and intuitive way to trace the path that data packets take from your device to a destination on the internet. With Paragon, you can quickly visualize the network route and identify any potential issues or bottlenecks.

## Features

- **Interactive Visualization**: Paragon generates a visual representation of the route taken by data packets, allowing users to see each hop along the way.
  
- **Geolocation Information**: Gain insights into the geographical locations of the network hops, helping to identify the physical locations of servers or routers.

TO be CONTINUED

## Installation

To install Paragon, follow these simple steps:

1. Clone the Paragon repository from GitHub:

    ```
    git clone https://github.com/11artyom11/paragon.git
    ```

2. Navigate to the Paragon directory:

    ```
    cd paragon
    ```

3. Install the required dependencies:

    ```
    make all
    ```

## Usage

Using Paragon is straightforward:

1. Enter the destination address or domain name you want to trace in the input field.
2. Click on the "Trace" button.
3. Paragon will start tracing the route, and you will see the visual representation generated on the screen.
4. Explore the visualization by zooming in/out, dragging, or hovering over nodes for more information.

## Example

Here's a simple example of how to use Paragon:
```
./paragon 3.11.88.57 
#You can set your desired IP address
```
After running this command you will see globe window opened and similar picture...

## Video Demo

Watch a demo of Paragon in action:

[![Watch the video](misc/paragon_screenshot)](misc/demo_paragon.mp4)

## Contributing

I welcome contributions to Paragon! If you find any bugs or have ideas for new features, feel free to open an issue or submit a pull request on our GitHub repository.

## License

Paragon is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contact

For any inquiries or support, please contact us at [grigorianartyom1@gmail.com](mailto:grigorianartyom1@gmail.com).
