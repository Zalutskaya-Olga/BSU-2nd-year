import org.w3c.dom.*;
import javax.xml.parsers.*;
import java.io.File;
import java.time.LocalDate;
import java.util.*;

class DayForecast {
    private LocalDate date;
    private int temperature;
    private String weather;

    public DayForecast(LocalDate date, int temperature, String weather) {
        this.date = date;
        this.temperature = temperature;
        this.weather = weather;
    }

    public LocalDate getDate() {
        return date;
    }

    public int getTemperature() {
        return temperature;
    }

    public String getWeather() {
        return weather;
    }

    @Override
    public String toString() {
        return "Дата: " + date + ", Температура: " + temperature + "°C, Погода: " + weather;
    }
}

public class WeatherParser {
    private Map<String, List<DayForecast>> forecastData;
    private Map<String, Boolean> cityWarnings;

    public WeatherParser() {
        forecastData = new HashMap<>();
        cityWarnings = new HashMap<>();
    }

    public void parseXML(String fileName) {
        try {
            File xmlFile = new File(fileName);
            DocumentBuilderFactory factory = DocumentBuilderFactory.newInstance();
            DocumentBuilder builder = factory.newDocumentBuilder();
            Document document = builder.parse(xmlFile);
            document.getDocumentElement().normalize();

            NodeList cityNodes = document.getElementsByTagName("city");

            for (int i = 0; i < cityNodes.getLength(); i++) {
                Node cityNode = cityNodes.item(i);
                if (cityNode.getNodeType() == Node.ELEMENT_NODE) {
                    Element cityElement = (Element) cityNode;
                    String cityName = cityElement.getAttribute("name");
                    String warningsAttr = cityElement.getAttribute("warnings");
                    boolean hasWarnings = warningsAttr.equals("yes");

                    List<DayForecast> cityForecasts = new ArrayList<>();

                    NodeList dayNodes = cityElement.getElementsByTagName("day");
                    for (int j = 0; j < dayNodes.getLength(); j++) {
                        Node dayNode = dayNodes.item(j);
                        if (dayNode.getNodeType() == Node.ELEMENT_NODE) {
                            Element dayElement = (Element) dayNode;
                            LocalDate date = LocalDate.parse(dayElement.getAttribute("date"));
                            int temp = Integer.parseInt(dayElement.getElementsByTagName("temp").item(0).getTextContent());
                            String weather = dayElement.getElementsByTagName("weather").item(0).getTextContent();

                            cityForecasts.add(new DayForecast(date, temp, weather));
                        }
                    }

                    forecastData.put(cityName, cityForecasts);
                    cityWarnings.put(cityName, hasWarnings);
                }
            }

            System.out.println("Данные успешно загружены из " + fileName);
            System.out.println("Загружено городов: " + forecastData.size());

        } catch (Exception e) {
            System.out.println("Ошибка при парсинге: " + e.getMessage());
            e.printStackTrace();
        }
    }

    public double getAverageTemperatureForCity(String cityName) {
        if (!forecastData.containsKey(cityName)) {
            System.out.println("Город '" + cityName + "' не найден.");
            return 0;
        }

        List<DayForecast> forecasts = forecastData.get(cityName);
        if (forecasts.isEmpty()) {
            return 0;
        }

        int sum = 0;
        for (DayForecast forecast : forecasts) {
            sum += forecast.getTemperature();
        }

        return (double) sum / forecasts.size();
    }

    public double getAverageTemperatureForCityPeriod(String cityName, LocalDate startDate, LocalDate endDate) {
        if (!forecastData.containsKey(cityName)) {
            System.out.println("Город '" + cityName + "' не найден.");
            return 0;
        }

        List<DayForecast> forecasts = forecastData.get(cityName);
        if (forecasts.isEmpty()) {
            return 0;
        }

        int sum = 0;
        int count = 0;

        for (DayForecast forecast : forecasts) {
            LocalDate date = forecast.getDate();
            if (!date.isBefore(startDate) && !date.isAfter(endDate)) {
                sum += forecast.getTemperature();
                count++;
            }
        }

        if (count == 0) {
            System.out.println("Нет данных для указанного периода.");
            return 0;
        }

        return (double) sum / count;
    }

    public Map.Entry<String, DayForecast> findWarmestDay() {
        DayForecast warmest = null;
        String warmestCity = null;
        int maxTemp = Integer.MIN_VALUE;

        for (Map.Entry<String, List<DayForecast>> entry : forecastData.entrySet()) {
            String cityName = entry.getKey();
            List<DayForecast> cityForecasts = entry.getValue();

            for (DayForecast forecast : cityForecasts) {
                if (forecast.getTemperature() > maxTemp) {
                    maxTemp = forecast.getTemperature();
                    warmest = forecast;
                    warmestCity = cityName;
                }
            }
        }

        if (warmest != null && warmestCity != null) {
            return new AbstractMap.SimpleEntry<>(warmestCity, warmest);
        }

        return null;
    }

    public void displayAllData() {
        System.out.println("\nВсе данные о погоде");
        for (Map.Entry<String, List<DayForecast>> entry : forecastData.entrySet()) {
            String cityName = entry.getKey();
            boolean hasWarnings = cityWarnings.get(cityName);
            System.out.println("\nГород: " + cityName + " (Предупреждения: " + (hasWarnings ? "ДА" : "нет") + ")");
            for (DayForecast forecast : entry.getValue()) {
                System.out.println("  " + forecast);
            }
        }
    }

    public void displayCityData(String cityName) {
        if (!forecastData.containsKey(cityName)) {
            System.out.println("Город '" + cityName + "' не найден.");
            return;
        }

        boolean hasWarnings = cityWarnings.get(cityName);
        System.out.println("\nДанные для города: " + cityName);
        System.out.println("Предупреждения: " + (hasWarnings ? "ДА" : "нет"));

        List<DayForecast> forecasts = forecastData.get(cityName);
        for (DayForecast forecast : forecasts) {
            System.out.println("  " + forecast);
        }
        double avgTemp = getAverageTemperatureForCity(cityName);
        System.out.printf("Средняя температура за весь период: %.2f°C\n", avgTemp);
    }

    public void displayCitiesWithWarnings() {
        System.out.println("\nГорода с предупреждениями");
        boolean found = false;

        for (Map.Entry<String, Boolean> entry : cityWarnings.entrySet()) {
            if (entry.getValue()) {
                String cityName = entry.getKey();
                System.out.println("- " + cityName);

                List<DayForecast> forecasts = forecastData.get(cityName);
                for (DayForecast forecast : forecasts) {
                    System.out.println("  Дата: " + forecast.getDate() + ", Погода: " + forecast.getWeather());
                }
                found = true;
            }
        }

        if (!found) {
            System.out.println("Нет городов с предупреждениями.");
        }
    }

    public static void main(String[] args) {
        WeatherParser parser = new WeatherParser();
        parser.parseXML("weather.xml");

        parser.displayAllData();

        Scanner scanner = new Scanner(System.in);

        System.out.println("\nАнализ данных");
        System.out.println("Доступные команды:");
        System.out.println("  'период' - средняя температура для города за период");
        System.out.println("  'самый теплый' - найти самый теплый день");
        System.out.println("  'список' - показать все данные");
        System.out.println("  'предупреждения' - показать города с предупреждениями");
        System.out.println("  'выход' или 'exit' - завершить программу");

        while (true) {
            System.out.print("\nВведите команду или название города: ");
            String input = scanner.nextLine().trim();

            if (input.equalsIgnoreCase("выход") || input.equalsIgnoreCase("exit")) {
                System.out.println("Программа завершена.");
                break;
            } else if (input.equalsIgnoreCase("самый теплый")) {
                System.out.println("\nПоиск самого теплого дня:");
                Map.Entry<String, DayForecast> warmestEntry = parser.findWarmestDay();
                if (warmestEntry != null) {
                    String cityName = warmestEntry.getKey();
                    DayForecast warmestDay = warmestEntry.getValue();
                    System.out.println("Самый теплый день:");
                    System.out.println("Город: " + cityName);
                    System.out.println("Данные: " + warmestDay);
                } else {
                    System.out.println("Данные не найдены.");
                }
            } else if (input.equalsIgnoreCase("список")) {
                parser.displayAllData();
            } else if (input.equalsIgnoreCase("предупреждения")) {
                parser.displayCitiesWithWarnings();
            } else if (input.equalsIgnoreCase("период")) {
                System.out.print("Введите название города: ");
                String city = scanner.nextLine().trim();
                System.out.print("Введите начальную дату (гггг-мм-дд): ");
                String startDateStr = scanner.nextLine().trim();
                System.out.print("Введите конечную дату (гггг-мм-дд): ");
                String endDateStr = scanner.nextLine().trim();

                try {
                    LocalDate startDate = LocalDate.parse(startDateStr);
                    LocalDate endDate = LocalDate.parse(endDateStr);

                    if (startDate.isAfter(endDate)) {
                        System.out.println("Ошибка: начальная дата должна быть раньше конечной.");
                    } else {
                        double avgTemp = parser.getAverageTemperatureForCityPeriod(city, startDate, endDate);
                        System.out.printf("\nСредняя температура в городе %s за период с %s по %s: %.2f°C\n",
                                city, startDate, endDate, avgTemp);
                    }
                } catch (Exception e) {
                    System.out.println("Ошибка при вводе даты. Используйте формат гггг-мм-дд.");
                }
            } else if (!input.isEmpty()) {
                System.out.println("\nАнализ для города: " + input);
                parser.displayCityData(input);
            }
        }

        scanner.close();
    }
}