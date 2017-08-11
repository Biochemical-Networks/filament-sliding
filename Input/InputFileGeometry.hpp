#ifndef INPUTFILEGEOMETRY_HPP
#define INPUTFILEGEOMETRY_HPP

/* Put the InputFileGeometry in global variables,
 * because this way these are grouped together.
 * Furthermore, it is not necessary to write a separate InputFileLine class
 * just for input/output of one line in the file.
 */

namespace InputFileGeometry
{
    /* Set the widths of the columns of the input file, used when a line is written while restoring a default input file.
     * constexpr explicitly shows that these are compile time constants, even though it should be the same as using const.
     * Static sets file scope.
     */
    static constexpr int nameWidth = 40;
    static constexpr int valueWidth = 20;
    static constexpr int unitWidth = 20;
    static constexpr int typeWidth = 20;
}

#endif // INPUTFILEGEOMETRY
