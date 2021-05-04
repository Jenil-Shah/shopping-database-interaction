-- Name: Jenil Shah

SET SERVEROUTPUT ON

-- Procedure to find the customers
CREATE OR REPLACE PROCEDURE find_customer (customerId IN NUMBER, found OUT NUMBER) AS
BEGIN
    SELECT COUNT(*)
        INTO found
    FROM customers c
    WHERE c.customer_id = customerId;
    
EXCEPTION
    WHEN NO_DATA_FOUND
        THEN
            DBMS_OUTPUT.PUT_LINE ('No Data Found!');
            found := 0;
    WHEN TOO_MANY_ROWS
        THEN
            DBMS_OUTPUT.PUT_LINE ('Too Many Rows Returned!');
    WHEN OTHERS
        THEN
            DBMS_OUTPUT.PUT_LINE('Error!');
END find_customer;

-- Procedure to find the product
CREATE OR REPLACE PROCEDURE find_product (productId IN NUMBER, price OUT products.list_price%TYPE) AS
BEGIN
    SELECT p.list_price
        INTO price
    FROM products p
    WHERE p.product_id = productId;
    
EXCEPTION
    WHEN NO_DATA_FOUND
        THEN
            DBMS_OUTPUT.PUT_LINE ('No Data Found!');
            price := 0;
    WHEN TOO_MANY_ROWS
        THEN
            DBMS_OUTPUT.PUT_LINE ('Too Many Rows Returned!');
    WHEN OTHERS
        THEN
            DBMS_OUTPUT.PUT_LINE('Error!');
END find_product;

-- Procedure to add order
CREATE OR REPLACE PROCEDURE add_order (customerId IN NUMBER, new_order_id OUT NUMBER) AS
BEGIN
    SELECT MAX(o.order_id)
        INTO new_order_id
    FROM orders o;
    
    new_order_id := new_order_id + 1;
    
    INSERT INTO orders
    VALUES(new_order_id,customerId,'Shipped',56,SYSDATE);
    
EXCEPTION
    WHEN OTHERS
        THEN
            DBMS_OUTPUT.PUT_LINE('Error!');
END add_order;

-- Procedure to add order item
CREATE OR REPLACE PROCEDURE add_order_item (orderId IN order_items.order_id%type,
                                            itemId IN order_items.item_id%type, 
                                            productId IN order_items.product_id%type, 
                                            qty IN order_items.quantity%type,
                                            price IN order_items.unit_price%type) AS
BEGIN
    INSERT INTO order_items
    VALUES(orderId,itemId,productId,qty,price);
    
EXCEPTION
    WHEN OTHERS
        THEN
            DBMS_OUTPUT.PUT_LINE('Error!');
END add_order_item;
