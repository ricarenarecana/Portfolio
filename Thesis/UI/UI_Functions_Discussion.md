# User Interface Functions Discussion

This section discusses the major functions of the RAON vending machine user interface based on the screenshots provided in the `UI` folder. The interface is designed to support two main user groups: customers who purchase electronic components through kiosk mode, and administrators who configure, monitor, and maintain the machine through admin mode.

## 1. Operating Mode Selection

The screen shown in `643958537_3079463708921134_2219463100842198941_n.png` serves as the entry point of the system. It provides two options: **Kiosk** and **Admin**. This function separates customer-facing operations from maintenance and configuration tasks. By doing so, the system protects administrative tools from accidental access while keeping the customer workflow simple and direct.

## 2. Customer Welcome and Instructions

The interface in `644908562_3846586795472100_4711215038945226382_n.png` acts as the welcome screen for customers. It explains the purchasing process step by step, from starting an order to scanning the QR code after the transaction. This function is important because it reduces user confusion, especially for first-time users, and sets clear expectations for payment, change, and item release.

## 3. Product Browsing and Item Selection

The main shopping interface shown in `643898971_1418292106446217_442417867031556788_n.png` allows customers to browse available electronic components. Products are grouped by category, such as Digital Electronics, Electronics, General, and Power Supply. Each item card displays the product image, name, category, and price, while stock labels such as **OK** and **Low** provide immediate feedback on availability.

This screen performs several functions at once:

- It helps users find items by category.
- It presents basic product information needed for selection.
- It informs users of stock condition before they attempt to purchase.
- It provides direct access to the shopping cart.

Because the items are visually organized into cards, the interface supports faster selection and improves readability.

## 4. Cart Management

Cart-related functions are shown in `643858033_961307559791887_6078312150771139094_n.png` and `646404031_1110398867880122_2955395453316644602_n.png`. The first image shows the empty cart state, while the second shows a cart containing a selected item with quantity controls and a remove option.

The cart interface allows the customer to:

- Review selected items before payment.
- Increase or decrease item quantity.
- Remove items from the order.
- View the running total cost.
- Return to shopping or proceed to payment.

This function is essential because it gives the customer control over the order before money is inserted, reducing input mistakes and unwanted purchases.

## 5. Payment Processing

The payment interface in `643870112_1628759011651373_9020708239575537005_n.png` handles the monetary part of the transaction. It clearly shows the **amount required**, the value of **coins inserted**, **bills inserted**, **total received**, and the **remaining balance**. It also lists the accepted denominations and the items being paid for.

One notable feature of this screen is the warning that cancelled payments do not return inserted coins or bills. This is a critical operational reminder that helps prevent misuse and informs users of the transaction rules before continuing. The screen also supports both navigation back to shopping and payment cancellation, which gives users controlled exit points during the transaction.

## 6. Administrative Item Management

The screen in `646738132_2101843374103326_2225731369212744285_n.png` shows the **Manage Items** interface. This function is intended for system administrators and operators. It displays the current inventory list together with item category, price, and quantity. It also includes buttons for editing and removing items.

In addition, this interface provides quick access to related maintenance functions such as:

- Editing coin settings
- Viewing logs
- Assigning slots
- Configuring the kiosk

The display of coin stock status and total change value is especially important because it allows the operator to determine whether the machine can provide change during transactions.

## 7. Coin Hopper Setup

The **Coin Hopper Setup** screen in `649704640_1253780402937488_8020113807756334436_n.png` is used to configure the available quantity and low-threshold values for coin denominations. In the screenshot, the denominations shown are `P1` and `P5`.

This function supports the machine's change-dispensing capability by allowing the administrator to:

- Define available coin stock
- Set low-threshold warning levels
- Save updated coin parameters

This is an important maintenance feature because accurate coin settings directly affect whether the machine can complete transactions and return the correct change.

## 8. Slot Assignment and Product Mapping

The interface in `650193957_897698326206498_2426567733208141690_n.png` shows the **Assign Items to Slots** function. This screen maps products to physical vending slots and displays the corresponding item, category, price, and quantity per slot. Each slot also includes controls such as **Edit**, **Test**, and **Clear**.

This function is significant for machine operation because it connects the digital inventory system to the physical storage mechanism. Through this screen, the administrator can:

- Assign items to specific slots
- Verify the correctness of assignments
- Test slot behavior
- Remove incorrect mappings

The presence of a test option suggests that the interface supports validation before the machine is made available to customers.

## 9. Kiosk Configuration

The **Kiosk Configuration** window shown in `645654624_1402627501638085_7066296006225683967_n.png` allows administrators to customize the system's identity and display settings. The visible fields include machine name, machine subtitle, header logo path, display diagonal size, and item categories.

This function supports flexibility and deployment readiness because it allows the kiosk interface to be tailored to the actual machine setup and branding requirements. It also suggests that the system can be reused in different contexts without changing the core program logic.

## 10. Sales and Temperature Monitoring

Three screenshots, `649290444_1538350547249672_6188665033185982001_n.png`, `645671152_2064603077440765_2054578363702311066_n.png`, and `644046298_944575304771774_2570216403242901664_n.png`, show the **Sales & Temperature Logs** module.

This module appears to provide three major functions:

- **Today's Summary**: presents a summary of daily transactions and sold items
- **View Logs**: displays detailed time-stamped temperature and relay records
- **History**: lists previously saved log files by date

This monitoring function is valuable because it combines commercial and technical oversight in one place. The sales summary helps evaluate usage and demand for specific components, while the temperature log helps ensure that the machine's internal environment remains within safe operating conditions. The historical log list also supports record keeping and future analysis.

## 11. Real-Time System Status Display

Several screenshots include a bottom status panel that shows environmental readings, TEC cooler status, IR sensor states, system status, and uptime. This persistent panel serves as a real-time monitoring feature for both kiosk and admin views.

Its functions include:

- Displaying temperature and humidity values
- Showing the operating state of the TEC cooler
- Reporting IR sensor conditions
- Confirming overall system operational status
- Tracking uptime

This constant visibility improves situational awareness and helps operators detect issues without leaving the current screen.

## 12. Overall Functional Assessment

Based on the screenshots, the RAON user interface is structured to support the complete workflow of an automated electronics vending machine. On the customer side, it covers product browsing, cart management, payment, and guided transaction completion. On the administrative side, it supports item management, slot assignment, coin configuration, kiosk customization, and system monitoring.

Overall, the interface demonstrates a practical separation between operational functions and maintenance functions. This design improves usability, strengthens control over machine settings, and supports both daily transactions and long-term system management.
