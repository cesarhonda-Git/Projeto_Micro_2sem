#include <zephyr/drivers/gpio.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/kernel.h>

/* LED configuration */
#define LED0_NODE DT_ALIAS(led0)
static const struct gpio_dt_spec led = GPIO_DT_SPEC_GET(LED0_NODE, gpios);

static const struct device *uart0_dev;

static void uart0_cb(const struct device *dev, void *user_data) {
    uint8_t c;

    if (!uart_irq_update(dev)) {
        return;
    }

    /* Clear errors */
    uart_err_check(dev);

    if (uart_irq_rx_ready(dev)) {
        while (uart_fifo_read(dev, &c, 1) == 1) {

            /* Echo back on uart0 */
            uart_poll_out(dev, c);

            /* Blink LED */
            gpio_pin_toggle_dt(&led);
        }
    }
}

void main(void) {
    /* Configure LED */
    if (!gpio_is_ready_dt(&led)) {
        return;
    }
    gpio_pin_configure_dt(&led, GPIO_OUTPUT_ACTIVE);

    /* Get uart0 */
    uart0_dev = DEVICE_DT_GET(DT_NODELABEL(uart0));

    if (!device_is_ready(uart0_dev)) {
        printk("uart0 não está pronta!\n");
        return;
    }

    /* Configure uart0 callback */
    uart_irq_callback_user_data_set(uart0_dev, uart0_cb, NULL);
    uart_irq_rx_enable(uart0_dev);

    while (1) {
        k_sleep(K_FOREVER);
    }
}
