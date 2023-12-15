# Real Caustics Engine для Blender

## Обзор
Real Caustics Engine – это рендеринговый движок, специализирующийся на рендеринге исключительно каустики (caustics) с использованием слияния вершин (Vertex Merging). Он разработан для интеграции с Blender с помощью [плагина](https://github.com/gregoryginzburg/BlenderRealCaustics).

# Пример работы
![alt text](https://github.com/gregoryginzburg/RealCausticsEngine/blob/Vertex_Merging/examples/example.png)


## Возможности
- **Слияние Вершин (Vertex Merging)**: SOT (State of the art) для симуляции каустики.
- **Разнообразные Источники Света**: Поддержка различных типов освещения.
- **Обработка Материалов и Мешей**: Поддержка различных материалов и мешей.
- **BVH Структуры Ускорения**: Оптимизировано для быстрого рендеринга.

## Начало Работы
### Предварительные Требования
- Visual Studio 2019
- Совместимость с Windows

### Установка
1. Клонируйте репозиторий:
   ```
   git clone https://github.com/gregoryginzburg/RealCausticsEngine.git
   ```
2. Откройте `Real Caustics.sln` в Visual Studio и скомпилируйте.

### Использование
После компиляции поместите полученный `Real Caustics.dll` файл в папку `engine` в [плагине](https://github.com/gregoryginzburg/BlenderRealCaustics) для Blender.

## Документация
- **Структура Исходного Кода**:
  - `src`: Все исходные файлы.
    - `BVH`, `KD_Tree`: Структуры пространственного разбиения.
    - `Scene`, `Camera`, `Lights`: Основные компоненты рендеринга сцен.
    - `materials`, `mesh`: Обработка различных материалов и мешей.
    - `Vertex_Merging`: Реализация алгоритма слияния вершин.
    - `main.cpp`: Точка входа в приложение.
  - `lib`: Используемые внешние библиотеки.
