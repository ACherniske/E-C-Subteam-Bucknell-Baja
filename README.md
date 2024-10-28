[![KiCad](https://img.shields.io/badge/Program-KiCad-blue?logo=data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAABcAAAAWCAYAAAArdgcFAAAAAXNSR0IArs4c6QAAAYBJREFUSEu1leFRAkEMhd+rQK1AOhArECtQKlAr0A6UDujAswKlAqUCsQPpgA7iPGZzEzJ7N3CD+2vvcnybfUkeNDPD/6wZjwRfA2gAbAA8ATgH0MJ/SiDe4SpdaFm5oH8zJfmhuJmNAXxH+ILkrf/YzCYAPgNsTXIU4WamTO/KuzOSynq7zEz7ucuyJCmgB18APAfYA0nBPD4H8FiLm5mSfI+Z98HfSN4HsPavFYkkmzK+KbFW8y64ajHxKxc9vwCc7NFgvXBVXeBV0fEUgPbqhLymqSFUr174yjugwJVx7iA/5Jqk4l4TzU4dntMys1zA/MkweOjbPpmHwfeQRJ/sDzezEcnfoKGKqeeuLjkIriHakJTWXqQ8tVGmg+Ga0EtvxSJPV2EHweV24+Qb6vWLVN1BcDGyqcnAdEDUfzBcB7SWWuTJ/tILz9m5s/ntd7ynHBAtN9dmZ0IX5aoOU+tFS9X7WWWK5D9abVeVZ92sOdbfXG16t97SGk7ffA+INX+jAkjEyroq/QAAAABJRU5ErkJggg==)](https://www.kicad.org/)
[![Arduino](https://img.shields.io/badge/Platform-Arduino-96D9D9?logo=arduino)](https://arduino.cc/)
[![Adafruit](https://img.shields.io/badge/Hardware-Adafruit-darkgray?logo=adafruit)](https://www.adafruit.com/)
# E&C Subteam Documentation Bucknell Baja Car
Wiring Harness, Controls, and Data Aquisition work for Bucknell Baja Electronics and Controls Subteam

------------------------------------------------------------------
<details>
<summary> Background </summary>

## What is Baja SAE?
Baja SAE is a globally recognized engineering competition hosted by SAE International, where university teams design, build, and test single-seat, off-road vehicles. These vehicles must endure rugged terrain and demonstrate capabilities in traction, stability, endurance, and maneuverability.

The Baja SAE competition emphasizes practical engineering skills, teamwork, and project management as students navigate the real-world challenges of designing, manufacturing, and testing a functional, durable vehicle. Participants engage in all stages of vehicle development, from concept to fabrication, while adhering to competition guidelines and safety requirements.

Teams compete in various events, including acceleration, hill climbs, and a grueling endurance race that tests the vehicle’s reliability over long distances. The competition allows engineering students to apply and expand their knowledge in areas like mechanical design, electronics, and control systems—developing crucial skills for their future careers.

[**Official Rulebook**](BAJA_RULES_2024_Rev_A_2023-08-31.pdf)  

</details>

## Scope
This documentation provides an overview of the Electronics and Controls systems implemented in our Baja SAE vehicle, in accordance with relevant sections of the Baja SAE 2024 rulebook. The systems described here prioritize safety, reliability, and regulatory compliance, ensuring that all electronic and control functionalities meet competition standards. Our scope specifically includes the following components:

Electrical System (B.10): The vehicle includes a minimum electrical setup as required by rule B.10, encompassing dual engine kill switches, a brake light, and a main battery for system power. All safety and signaling systems are designed to function independently of the kill switches to maintain continuous operation during vehicle shutdowns.

Battery Requirements (B.10.1.1): We selected and mounted a factory-sealed, maintenance-free battery that remains securely fixed during normal operation and under collision forces, as specified in B.10.1.1.1. For systems powered by this battery, we conducted capacity assessments to confirm reliable performance throughout the event, fulfilling rule B.10.1.1.3.

Wiring and Connectors (B.10.2): All wiring is routed to avoid high-temperature or high-wear areas, supporting accessibility and maintaining safety per B.10.2. Wiring pathways are designed to prevent interference with driver egress and other critical components.

Kill Switches (B.10.3): In compliance with B.10.3, the vehicle has two kill switches for engine cutoff—one within the driver’s reach in the cockpit and an external one accessible for track workers. Both are firmly mounted and ensure instant engine shutdown during emergency stops.

Signaling Devices (B.10.4)**:

Brake Light: The vehicle uses an approved brake light positioned for visibility to trailing drivers, activated exclusively by a hydraulic pressure switch on the brake system, as outlined in rule B.10.4.1.
Data Acquisition (B.10.6): The vehicle is equipped with optional data acquisition systems to log performance metrics. These systems do not relay real-time feedback to the driver, and all associated costs are included in the official competition cost report.
